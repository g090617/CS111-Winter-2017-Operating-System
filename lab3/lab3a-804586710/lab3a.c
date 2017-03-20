#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h> 
#include <sys/types.h>
#include <sys/stat.h>

uint32_t block_size;
int32_t frag_size;
int num_group;

struct group_descriptor *group;

struct super_block super;


struct inode_table{
  uint16_t mode;
  uint16_t uid;
  uint32_t size;
  uint32_t atime;
  uint32_t ctime;
  uint32_t mtime;
  uint32_t dtime;
  uint16_t gid;
  uint16_t links_count;
  uint32_t blocks;
  uint32_t flags;
  uint32_t osd1;
  uint32_t block[15];
  uint32_t generation;
  uint32_t file_acl;
  uint32_t dir_acl;
  uint32_t faddr;
  uint32_t osd2[3];
} ;

struct super_block
{
  uint32_t s_inodes_count;
  uint32_t s_blocks_count;
  uint32_t s_r_blocks_count;
  uint32_t s_free_blocks_count;
  uint32_t s_free_inodes_count;
  uint32_t s_first_data_block;
  uint32_t s_log_block_size;
  int32_t s_log_frag_size;
  uint32_t s_blocks_per_group;
  uint32_t s_frags_per_group;
  uint32_t s_inodes_per_group;
  uint32_t s_mtime;
  uint32_t s_wtime;
  uint16_t s_mnt_count;
  uint16_t s_max_mnt_count;
  uint16_t s_magic;
};

struct group_descriptor
{
  uint32_t bg_block_bitmap;
  uint32_t bg_inode_bitmap;
  uint32_t bg_inode_table;
  uint16_t bg_free_blocks_count;
  uint16_t bg_free_inodes_count;
  uint16_t bg_used_dirs_count;
  
};


struct directory
{
  uint32_t inode;
  uint16_t rec_len;
  uint8_t name_len;
  uint8_t file_type;
  char name[255];
};

void parse_super_block(int fd)
{
  int super_fd = open("super.csv", O_WRONLY | O_CREAT, 0666);
  if(pread(fd, &super, sizeof(struct super_block), 1024) == -1)
    {
      fprintf(stderr,"Invalid pread\n");
      exit(1);
    }

 
  block_size = 1024 << super.s_log_block_size;
  if(super.s_log_frag_size > 0)
    frag_size = 1024 << super.s_log_frag_size;
  else
    frag_size = 1024 >> -super.s_log_frag_size;

  if(super.s_magic != 0xef53)
    {
      fprintf(stderr, "Invalid magic: %x\n", super.s_magic);
      exit(1);
    }

  if(block_size > 64000)
    {
      fprintf(stderr, "Invalid block size: %d > 64000\n", block_size);
      exit(1);
    }
  else if( block_size < 512)
    {
      fprintf(stderr, "Invalid block size: %d < 512\n", block_size);
      exit(1);
    }

  
  dprintf(super_fd,"%x,%d,%d,%d,%d,%d,%d,%d,%d\n",
	 super.s_magic,
	 super.s_inodes_count,
	 super.s_blocks_count,
	 block_size,
	 frag_size,
	 super.s_blocks_per_group,
	 super.s_inodes_per_group,
	 super.s_frags_per_group,
	 super.s_first_data_block);

  num_group = super.s_blocks_count/super.s_blocks_per_group;
  group = (struct group_descriptor *)malloc(num_group);
  //inode = (struct inode_table *)malloc(num_group);
}



void parse_group_descriptor(int fd)
{
  int group_fd = open("group.csv", O_WRONLY | O_CREAT, 0666);
  
  int i = 0;
  for(i = 0; i < num_group; i++)
    {
      pread(fd, &group[i], sizeof(struct group_descriptor), 2048 + i * 32);
      dprintf(group_fd, "%d,%d,%d,%d,%x,%x,%x\n", 
	     super.s_blocks_per_group,
	     group[i].bg_free_blocks_count,
	     group[i].bg_free_inodes_count,
	     group[i].bg_used_dirs_count,
	     group[i].bg_inode_bitmap,
	     group[i].bg_block_bitmap,
	     group[i].bg_inode_table);      
    }
}

void parse_bitmap(int fd)
{
  uint32_t block_byte = 0;
  uint32_t inode_byte = 0;
  char buf[1024];
  int i = 0;
  int j = 0;
  int m = 0;
  int block_count = 0;
  int inode_count = 0;
  int inode_pos = 0;
  int indirect_count = 0;

  int inode_fd = open("inode.csv", O_WRONLY | O_CREAT, 0666); 
  int direct_fd = open("directory.csv", O_WRONLY | O_CREAT, 0666);
  int indirect_fd = open("indirect.csv", O_WRONLY | O_CREAT, 0666);
  int bitmap_fd = open("bitmap.csv", O_WRONLY | O_CREAT, 0666);
  //FILE *bitmap_fd bitmap_fd = fopen("bitmap.csv", "w");
  int temp1 = 0;
  int temp2 = 0;
  for(m = 0; m < num_group; m++)
  {
    pread(fd,&buf,1024,group[m].bg_block_bitmap * 1024);
      for(j = 0; j < 1024; j++)
      {
	block_byte = buf[j];  
	//pread(fd,&block_byte,1,group[m].bg_block_bitmap * 1024 + j); 
	int k = 0;
	  
	  for(k = 0; k < 8; k++)
	    {
	      block_count++;
	      if((1 & (block_byte >> k)) == 0)
		{
		  temp1++;
		  dprintf(bitmap_fd,"%x,%d\n", 
		  	  group[m].bg_block_bitmap,  
		  	  block_count);
		  }
	      /*if((1 & (block_byte >> k)) == 0)
		{
		  temp1++;
		  //fprintf(bitmap_fd,"%x,%d\n",
		  //	  group[m].bg_block_bitmap,
		  //	  block_count);
		  }*/
	    }
      }
      int prev = 0;
      pread(fd,&buf,1024,group[m].bg_inode_bitmap * 1024);
      for(j = 0; j < 1024; j++)
	{
	  //pread(fd,&inode_byte,1,group[m].bg_inode_bitmap * 1024 + j); 
	  struct inode_table temp;
          inode_byte = buf[j];
	  int k = 0;
	  char type = '?';
          for(k = 0; k < 8; k++)
            {
              inode_count++;
              if((1 & (inode_byte >> k)) == 0)
                {
		  temp2++;
		  inode_count++; 
                  dprintf(bitmap_fd,"%x,%d\n",
		  	  group[m].bg_inode_bitmap,
		  	  m * super.s_inodes_per_group + k + 1 + j * 8);
		  /*fprintf(bitmap_fd,"%x,%d\n",
                          group[m].bg_inode_bitmap,
                          m * super.s_inodes_per_group + k + 1 + j * 8);*/
		}
	      else if(j == prev || j - 1 == prev || (m == 15) && (j == 62))
		{
		  prev = j;
		  if(group[m].bg_free_inodes_count != 2048)
		    {
		      //inode_count++;
		      pread(fd,
			    &temp,
			    sizeof(struct inode_table),
			    group[m].bg_inode_table * 1024 + (j * 8 + k) * 128 );
		      if(temp.mode >> 12 == 0x4)
			{
			  type = 'd';
			}
		      else if(temp.mode >> 12 == 0x8)
			{
			  type = 'f';
			}
		      else 
			{
			  type = '?';
			}
		      dprintf(inode_fd,"%d,%c,%o,%d,%d,%d,%x,%x,%x,%d,%d",
			      m * super.s_inodes_per_group + k + 1 + j * 8,
			      type,
			      temp.mode,
			      temp.uid,
			      temp.gid,
			      temp.links_count,
			      temp.ctime,
			      temp.mtime,
			      temp.atime,
			      temp.size,
			      (temp.blocks/(2 << block_size)));
		      int n = 0;
		      for(n = 0; n < 15; n++)
			dprintf(inode_fd, ",%x",temp.block[n]);
		      dprintf(inode_fd,"\n");
		    }
		  
		  //directories
		  if(type == 'd')
		    {
		      int entry_num = 0;
		      //printf("In type\n");
		      struct directory temp_dir;
		      int f_for = 0;
		      int s_for = 0;
		      int t_for = 0;
		      //char direct_buf[1024];
		      for(f_for = 0; f_for < 12; f_for++)
			{
			  if(temp.block[f_for] == 0)
			    break;
			  int pos = 0;
			  char *name;
			  while(pos < 1024)
			    {
			      pread(fd, &temp_dir, sizeof(struct directory),
			          temp.block[f_for] * 1024 + pos);
			      //temp_dir = direct_buf[pos];
			      temp_dir.name[temp_dir.name_len] = '\0';
			      if(temp_dir.name_len > 0 && temp_dir.inode > 0
				 && temp_dir.rec_len > 0)
				dprintf(direct_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
					m * super.s_inodes_per_group + k + 1 + j * 8,
					entry_num,
					temp_dir.rec_len,
					temp_dir.name_len,
					temp_dir.inode,
					temp_dir.name);
			      pos = pos + temp_dir.rec_len;
			      entry_num++;
			    }
			  
			}
		      //printf("m is %d\n",m);
		      if(temp.block[12] > 0)
			{
			int num_pointer = 0;
			for(f_for = 0; f_for < 256; f_for++)
			  {
			    int pointer = 0;
			    pread(fd, &pointer, sizeof(int), 
				  temp.block[12] * 1024 + 4 * f_for);
			    if(pointer == 0)
			      break;
			    
			    num_pointer++;
			    
			    int pos = 0;
			    char *name;
			    while(pos < 1024)
			      {
				pread(fd, &temp_dir, sizeof(struct directory),
				      pointer * 1024 + pos);
				temp_dir.name[temp_dir.name_len] = '\0';
				if(temp_dir.name_len > 0 && temp_dir.inode > 0
				   && temp_dir.rec_len > 0)
				  {
				  //printf("parent is %x\n",temp.block[12]);
				    dprintf(direct_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
					    m * super.s_inodes_per_group + k + 1 + j * 8,
					    entry_num,
					    temp_dir.rec_len,
					    temp_dir.name_len,
					    temp_dir.inode,
					    temp_dir.name);
				    //printf("%x,%x\n",temp.block[12],pointer);
				    
				  }
				pos = pos + temp_dir.rec_len;
				entry_num++;
				//printf("m is %d\n",m);
				
			      }
			    dprintf(indirect_fd,"%x,%d,%x\n",
				    temp.block[12],indirect_count,pointer);
			    indirect_count++;
			    //printf("pointer is %d\n",pointer);
			  }
		      }
		      }
		      }
	    }
	  
	}            
  }
  //printf("temp1 %d temp2 %d\n",temp1,temp2);
  //printf("block %d inode %d\n",block_count, inode_count);
}





int main(int argc, char *argv[])
{
  char *file_name = argv[1];
  int file_descriptor;
  //printf("%s\n", file_name);

  file_descriptor = open(file_name, O_RDONLY);
  if(file_descriptor == -1)
    {
      fprintf(stderr, "Can't open file\n");
      exit(1);
    }
  printf("This may take about 3 minutes, please be patient :)\n");
  parse_super_block(file_descriptor);
  parse_group_descriptor(file_descriptor);
  parse_bitmap(file_descriptor);
}
