/*
    File: my_allocator.h

    Original Author: R.Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/08

    Modified:

 */

#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include<vector>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
	// decide what goes here
	// hint: obviously block size will go here
public:
	int block_size;
	bool is_used;
	bool is_left;
	BlockHeader* next;

BlockHeader(int b_size)
{
	block_size = b_size;
	is_used = false;
	next = NULL;
	is_left = true;
}
~BlockHeader();
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeader s.
private:
	BlockHeader* head;		// you need a head of the list
public:
	LinkedList()
	{
		  head = NULL;
	}
	BlockHeader * get_head(){return head;}
	void insert (BlockHeader* b)
	{	// adds a block to the list
		if(b == NULL)
		{
			head = NULL;
		}

		else if(head == NULL)
		{
			//cout<<"making head"<<endl;
			head  =  b;
			head->next = NULL;
		}
		else
		{
			b->next = head;
			//head->next = NULL;
			head = b;
		}

	}

	void remove (BlockHeader* b)
	{  // removes a block from the list
		//cout<<"came to remove"<<endl;
		BlockHeader *curr= head,*prev = NULL;
		if(curr == NULL)
		{
			//cout<<"curr == null"<<endl;
			//cout<<"empty cant remove"<<endl; //list is null cant remove anything
		}
		else
		{
			if(curr == b) //b is head
			{
				//cout<<"curr == b"<<endl;
			//	curr = curr->next;

			 head = curr->next;
			}

			else
			{
				while(curr != NULL)
				{
					prev = curr;
					curr = curr->next;
					if(curr == b)
					{
						//cout<<"curb"<<endl;
						prev->next = curr->next;
					}
					curr = NULL;
				}
			}
		}
	}

	int list_size()
	{
		BlockHeader *p =head;
		int count=0;
		if(p == NULL)
		{
			return 0;
		}
		else
		{
			while(p != NULL)
			{
				count++;
				p = p->next;
			}
			return count;
		}
	}
};




class BuddyAllocator{
private:
	/* declare member variables as necessary */
	char * memory;
	int max_length,min_length;
	vector<LinkedList*> freelist;


private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	 //tells which list of the freelist vector block should belong to
	 int get_list_num(int bsz)
	 {
		 int i=0 ;
		 int l = max_length;
		 while(i < freelist.size() && l != bsz )
			 {
				 l = l/2;
				 i++;
			 }
		 if(i>=freelist.size())
		 	i = freelist.size() - 1;


		 return i;

	 }

//gives upper power of 2
 int pwr_ceil(int length)
 {
	 int upper = 1;
	 while(upper < length)
	 {
		 upper *= 2;
	 }
	 if(upper < min_length)
	 {
		 upper = min_length;
		 return upper;
	 }
	 //cout<<"upper: "<<upper<<endl;
	 return upper;
 }

	BlockHeader* getbuddy (BlockHeader * addr)
	{
		char* address = (char*)(addr);
		char* buddy_address;
		int current_block_size = addr->block_size;
		if(current_block_size == max_length)
		{
			//first block wont have a Buddy
			return NULL;
		}

		else if(addr->is_left == true)
		{
			//cout<<"left block"<<endl;
			buddy_address = (char*)(address + current_block_size);
		}
		else if(addr->is_left == false)
		{
		 //cout<<"right block"<<endl;
			buddy_address = (char*)(address - current_block_size);
		}

		BlockHeader *buddy  = (BlockHeader*)(buddy_address);
		//cout<<"buddy in buddy: "<<buddy->block_size<<endl;

			return buddy;
	}
	// given a block address, this function returns the address of its buddy

	bool arebuddies (BlockHeader* block1, BlockHeader* block2)
	{
		if(block1 == getbuddy(block2) && block1->block_size == block2->block_size)
		{
			return true;
		}
		return false;
	}
	// checks whether the two blocks are buddies are not

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2)
	{
		//update
		//char * h_ad = (char*)(header);
		//cout<<"in me"<<endl;
		if(block1->block_size == block2->block_size)
		{
			if(arebuddies(block1,block2) == true)
			{
				//cout<<"same size"<<endl;
			  if(block1->is_used == false && block2->is_used == false)
				{
					//cout<<"both av"<<endl;
					int new_block_size = (block1->block_size *2);
					char *merge_address;

					if(block1->is_left == true)
					{
						merge_address = (char*)(block1);
					}
					else if(block2->is_left == true)
					{
						merge_address = (char*)(block2);
					}

					BlockHeader * merged = new BlockHeader(new_block_size);
					merged = (BlockHeader*)(merge_address);
					merged->is_used = false;
					//merged->is_left = true;

					merged->block_size = new_block_size;

					return merged;

				}
				else
				{
					return NULL;
				}
			}
			else
			{
				return NULL;
			}
		}

		else
    {
			return NULL;
		}
	}
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	BlockHeader* split (BlockHeader* block)
	{
		//cout<<"came to split"<<endl;
		BlockHeader *current_block = block;
		char *current_address = (char*)(block); //give to left block
		int current_block_size = current_block->block_size;

		if(current_block->is_used == false)
		{
			current_block->is_used = true;
			int old_list = get_list_num(current_block_size);
			//current_block->is_used = true;
			freelist[old_list]->remove(current_block);
			// if(get_list_num(current_block_size) == 0)
			// {
			// 	freelist[get_list_num(current_block_size)]->remove(current_block);
			// }
			int new_block_size = (current_block_size /2);
			BlockHeader * right_block,*left_block;

			char *right_address = (char*)(current_address + new_block_size);

			left_block = (BlockHeader*)(current_address);
			left_block->block_size = new_block_size;
			left_block->is_left = true;
			left_block->is_used = false;

			right_block = new BlockHeader(new_block_size);
			right_block = (BlockHeader*)(right_address);
			right_block->block_size = new_block_size;
			right_block->is_left = false;
			right_block->is_used = false;
			//right_block->next = NULL;

			int split_list = get_list_num(new_block_size);
			freelist[split_list]->insert(right_block);
			freelist[split_list]->insert(left_block);

			return left_block;
			//left_block->next = right_block;
		}
		else
		{
		  //cant split
		//	cout<<"cant split"<<endl;
			return block;
		}


	}
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected


public:
	BlockHeader * header;
	BuddyAllocator (int _basic_block_size, int _total_memory_length);
	/* This initializes the memory allocator and makes a portion of
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as
	   its minimal unit of allocation. The function returns the amount of
	   memory made available to the allocator. If an error occurred,
	   it returns 0.
	*/

	~BuddyAllocator();
	/* Destructor that returns any allocated memory back to the operating system.
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/

	char* alloc(int _length);
	/* Allocate _length number of bytes of free memory and returns the
		address of the allocated portion. Returns 0 when out of memory. */

	int free(char* _a);
	/* Frees the section of physical memory previously allocated
	   using ’my_malloc’. Returns 0 if everything ok. */

	void debug ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	128: 5
	256: 0
	512: 3
	1024: 0
	....
	....
	 which means that at point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif
