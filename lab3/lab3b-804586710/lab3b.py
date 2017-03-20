import csv

#5.link count : inode.csv entry 6, directory entry 5
#4.missing inode: directory.csv ? after 10 rows ,go to bitmap 
#2.duplicated block: inode.csv enry 12 to27
#1.unallocated block: check all the block entries and find whether it is in bitmap.csv
#6.incorrect directory entry: in directory.csv if '.' column 1 should be the same as
#column 5, if '..' look at what parent is at column 1, skip '.' at column 5 entry
#and find the parent at column 5
#7. 
print 'this may take 15 seconds, please be patient :)'


inode_file = open('inode.csv','r')
direct_file = open('directory.csv','r')
bitmap_file = open('bitmap.csv','r')
group_file = open('group.csv','r')
output_file = open('lab3b_check.txt','w+')

direct_count = 0
inode_count = 0

inode_total = []
direct_total = []
bitmap_total = []
group_total = []

for group_line in group_file.readlines():
    group_line_array = group_line.split(',')
    group_total.append(group_line_array)
group_count = len(group_total)

block_pos = []
block_pos.append(0)
for i in range(1, group_count):
    block_pos.append(block_pos[i-1]+int(group_total[i-1][1]) + int(group_total[i-1][2]))

inode_pos = []
inode_pos.append(2048 - int(group_total[0][2]))
for i in range(1, group_count):
    inode_pos.append(inode_pos[i-1] + 2048 - int(group_total[i][2]))

#link count  
#missing inode
for inode_line in inode_file.readlines():
    inode_line_array = inode_line.split(',')
    inode_total.append(inode_line_array)
    link_count = inode_line_array[5]
    inode_num = inode_line_array[0]
    count = 0
    for direct_line in direct_file.readlines():
        direct_line_array = direct_line.split(',')
        if inode_count < 1:
            direct_total.append(direct_line_array)
            direct_count = direct_count + 1
        if direct_line_array[4] == inode_num:
           count = count + 1
    if count != int(link_count):
        output_file.write('LINKCOUNT < ' + str(inode_num) + \
                          ' > IS < ' + str(link_count) + ' > SHOULD BE < ' + \
                          str(count) + ' >\n')
        
    block_add = 0
    for i in range(11,26):  
        block_entry = int("0x" + inode_line_array[i],16)
        block_add = block_add + block_entry
        inode_total[inode_count][i] = block_entry
    if block_add == 0 and int(inode_num) > 10:
        for i in range(0,group_count):
            free_list = 0
            if inode_count < inode_pos[i]:
                free_list = int(group_total[i][4],16)
                break
        output_file.write('MISSING INODE < ' + str(inode_num) + \
                          ' > SHOULD BE IN FREE LIST < ' + str(free_list) + ' >\n')
    direct_file.seek(0)
    inode_count = inode_count + 1

done = 1
#duplicated block
temp = 0
head = 0
for i in range(0,inode_count):
    for j in range(11,26):
        element_1 = inode_total[i][j]
        if element_1 == 0:
            break
        for k in range(0,inode_count):
            for m in range(11,26):
                element_2 = inode_total[k][m]
                if element_2 == 0:
                    break
                #if i == 22 and j == 11:
                    #print hex(element_1),hex(element_2)
                if element_1 == element_2:
                    if i != k and inode_total[i][0] < inode_total[k][0]:
                        if temp != element_1:
                            if done != 1:
                                output_file.write('\n')
                            else:
                                done = 0
                            output_file.write('MULTIPLY REFERENCED BLOCK < ' + \
                                              str(element_1) + ' > BY INODE < ' + \
                                              str(inode_total[i][0]) + \
                                              ' > ENTRY < ' + str(j - 11) + \
                                              ' > INODE < ' + \
                                              str(inode_total[k][0]) + \
                                              ' > ENTRY < ' + str(m - 11) + ' >')
                            temp = element_1
                        else:
                            if inode_total[k][0] > head:
                                head = inode_total[k][0]
                                output_file.write(' INODE < ' + \
                                                  str(inode_total[k][0]) + \
                                                  ' > ENTRY < ' + str(m - 11) + ' >') 
output_file.write('\n')

#incorrect directory entry
idk = 0
for i in range(0,direct_count):
    fdot = int(ord(direct_total[i][5][1]))
    sdot = int(ord(direct_total[i][5][2]))
    if fdot == 46 and sdot == 46:
        element_1 = direct_total[i][0]
        Not_found = 0
        for j in range(0,i):
            if direct_total[j][4] == element_1 and \
               direct_total[i][4] == direct_total[j][0]:
                break
            elif direct_total[j][4] == element_1 and \
                 direct_total[i][4] != direct_total[j][0]:
                output_file.write('INCORRECT ENTRY IN < ' + str(direct_total[i][0]) + \
                                  ' > NAME < .. > LINK TO < ' + \
                                  str(direct_total[i][4]) + \
                                  ' > SHOULD BE < ' + str(direct_total[j][0]) + ' >\n')
    elif fdot == 46 and sdot == 34:
        if direct_total[i][0] != direct_total[i][4]:
            output_file.write('INCORRECT ENTRY IN < ' + direct_total[i][0] + \
                              ' > NAME < . > LINK TO < ' + direct_total[i][4] + \
                              ' > SHOULD BE < ' + direct_total[i][0] + ' >\n')


#invalid block
for i in range(0,inode_count):
    block_num = int(inode_total[i][10])
    #print block_num

    for j in range(11, 11 + block_num):
        if j < 24:
            if inode_total[i][j] == 0:
                output_file.write('INVALID BLOCK < ' + str(inode_total[i][j]) + \
                                  ' > IN INODE < ' + inode_total[i][0] + \
                                  ' > ENTRY < ' + \
                                  str(j - 11) + ' >\n')


for bitmap_line in bitmap_file.readlines():
    bitmap_line_array = bitmap_line.split(',')
    bitmap_total.append(bitmap_line_array)
bitmap_count = len(bitmap_total)


#unallocated block
#use information from group.csv

for i in range(0, inode_count):
    for j in range(11,26):
        element = inode_total[i][j]
        if element == 0:
            break
        #print 'processing block', element
        found = 0
        for k in range(1,group_count):
            if element <= int("0x"+group_total[k][5],16):
                lower = block_pos[k-1]
                higher = lower + int(group_total[k-1][1])
                for m in range(lower, higher):
                    if int(bitmap_total[m][1]) > element:
                        break
                    if int(bitmap_total[m][1]) == element:
                        output_file.write('UNALLOCATED BLOCK < ' + str(element) + \
                                          ' > REFERENCED BY INODE < ' + \
                                          str(inode_total[i][0]) + ' > ENTRY < ' + \
                                          str(j - 11) + ' >\n')
                        found = 1
                        break
            if found == 1:
                break

continuous_list = []
lower = 0
higher = 0
for i in range(1,inode_count):
    if int(inode_total[i][0]) - int(inode_total[i - 1][0]) == 1:
        if i == 1:
            lower = 0
        elif i == inode_count - 1:
            higher = int(inode_total[i][0])
            bound = [lower,higher]
            continuous_list.append(bound)
    else:
        higher = int(inode_total[i - 1][0])
        bound = [lower,higher]
        continuous_list.append(bound)
        lower = int(inode_total[i][0])

continuous_size = len(continuous_list)
for i in range(0,direct_count):
    found = 0
    for j in range(0,continuous_size):
        if int(direct_total[i][4]) <= continuous_list[j][1] or \
           int(direct_total[i][4]) >= continuous_list[j][0]:
            found = 1
            break
    if found == 0:
        output_file.write('UNALLOCATED INODE < ' + direct_total[i][3] + \
                          ' > REFERENCED BY < ' + direct_total[i][0] + \
                          ' > ENTRY < ' + direct_total[i][4] + ' >\n')
#print continuous_list
