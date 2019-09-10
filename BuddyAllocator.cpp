/*
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include<vector>
using namespace std;

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length)
{
		memory = new char[_total_memory_length]; //memory pool

		//about pool
		max_length = _total_memory_length;
		min_length = _basic_block_size;

		//freelist vector
		int num_of_lists = 1;
		int bl_size = _total_memory_length;
		while(bl_size != _basic_block_size)
		{
			bl_size = bl_size/2;
			num_of_lists++;
		}

		for(int i=0;i<num_of_lists;i++)
		{
			LinkedList * new_free_list = new LinkedList();
			freelist.push_back(new_free_list);
		}

		//create memory_block
		header = (BlockHeader*)(memory);
		header->next = NULL;
		header->block_size = _total_memory_length;
		header->is_left = true;

		freelist[0]->insert(header);

}

BuddyAllocator::~BuddyAllocator (){

	//delete dynamically allocated freelists
	int num_of_lists = freelist.size();
	for(int i=0;i<num_of_lists;i++)
	{
		delete freelist[i];
		freelist[i] = NULL;
	}

	//delete memory pool
	delete [] memory;
	memory = NULL;
}


char* BuddyAllocator::alloc(int _length) {
	/* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
	//find empty block  - split if needed
	//assign address
	//update free list
	//debug();
	int required_space = pwr_ceil(_length + sizeof(BlockHeader));
	//cout<<"required"<<required_space<<endl;
	if(required_space >= max_length)
	{
		required_space = max_length;
	}
	else
	{
		if((required_space <= min_length))
			required_space = min_length;
	}


	int list_num = get_list_num(required_space);
	//cout<<"going to allocate"<<list_num<<endl;
	int i = list_num;//list of required block size
	//cout<<"i: "<<i<<endl;
	//just making sure
  //cout<<"entering while"<<endl;

	if(i ==0)
	{
		BlockHeader *list_head = freelist[i]->get_head();
		if(list_head != NULL)
		{
			BlockHeader *used_block = list_head;
			char *header_address = (char*)(list_head);
			char *user_buff = (char*)(header_address + sizeof(BlockHeader));
			used_block->is_used = true;
			freelist[i]->remove(used_block);
			//cout<<"alloc done"<<endl;
			return user_buff;
		}
		else
		{
			//cout<<"not enough left"<<endl;
			return NULL;
			//break;
		}
	}

	while(i >=0 && i<freelist.size())
	{
		//cout<<"came to "<<i<<endl;
		BlockHeader *list_head = freelist[i]->get_head();
		if(list_head == NULL)
		{
			//cout<<"i: "<<i<<endl;
			i--;
		}
		else
		{
			//cout<<"entered"<<endl;
			if(list_head->block_size == required_space)
			{
				//no need to split
				//do allocation
				//correct block found

				BlockHeader *used_block = list_head;
				char *header_address = (char*)(list_head);
				char *user_buff = (char*)(header_address + sizeof(BlockHeader));
				used_block->is_used = true;
				freelist[i]->remove(used_block);
				//cout<<"alloc done"<<endl;
				return user_buff;
			}
			else
			{

			//	if(list_head->is_used == false)
			//	{
					//cout<<"here"<<endl;
					BlockHeader *h = list_head;
					int old_list = i;
					//cout<<"splitting"<<list_head->block_size<<endl;
					freelist[old_list]->remove(list_head);
					//cout<<"from: "<<old_list<<endl;
					BlockHeader * split_block = split(list_head);
					if(split_block == NULL)
					{
						//cout<<"split didnt happen"<<endl;
						// h->is_used = true;
						// freelist[old_list]->remove(h);

						i--; //move down and try splitting bigger blocks
					}
					else
					{

						//split happened
						//freelist[old_list]->remove(h);
						i++; //move back up
					}

			}
		}
	}
if(i >=  freelist.size())
{
	//cout<<"here"<<endl;
	return NULL;
}
 if(i == 0)
 {
	 //cout<<"not enough"<<endl;
	 return NULL;
 }
 else if(i<0)
 {

	 return NULL; //no space
 }


  //return new char [_length];
}

int BuddyAllocator::free(char* _a) {

	//debug();
	//cout<<"called free"<<endl;
	char* header_address = (char*)(_a - sizeof(BlockHeader));
	//cout<<"got header"<<endl;

	BlockHeader *freed_block = (BlockHeader*)(header_address);
	//cout<<freed_block->block_size<<endl;
	int freed_block_size = freed_block->block_size;
	//cout<<"freeing: "<<freed_block_size<<endl;
	if(freed_block_size > max_length)
		return 1;

	int list_num = get_list_num(freed_block_size);

	// cout<<"list_num: "<<list_num<<endl;
	// cout<<"here"<<endl;
	if(list_num == 0) //biggest memory block was freed ; wont have a buddy
	{
		//cout<<"in l0"<<endl;
		freed_block->is_used = false;
		freelist[list_num]->insert(freed_block);
		return 0;
	}
	else
	{
		//cout<<"in l"<<list_num<<endl;
	//	BlockHeader *fb = freed_block;
		//cout<<"done inserting"<<endl;
		freed_block->is_used = false;
		BlockHeader * buddy = getbuddy(freed_block);
		if(buddy != NULL)
		{
			//buddy exists
			int take_from,put_into;
			//cout<<"buddy exists"<<endl;
			if(buddy->is_used = true)
			{
				freed_block->is_used = false;
				freelist[list_num]->insert(freed_block);
				return 0;
			}
			BlockHeader * merged = buddy;
			while(merged != NULL )
			{
				//cout<<"here"<<endl;
				take_from = get_list_num(freed_block->block_size);
				//cout<<"take from "<<take_from<<endl;
				merged = merge(buddy,freed_block);
				if(merged == NULL)
					break;
			//	cout<<"ok:merged into  "<<merged->block_size<<endl;
				//merge successfull
				if(merged != NULL)
				{
					//cout<<"merge happened"<<endl;
					freed_block->is_used = true;
					buddy->is_used = true;
					freelist[take_from]->remove(freed_block);
					freelist[take_from]->remove(buddy);
				}
				put_into = get_list_num(merged->block_size);
			//	cout<<"pi: "<<put_into<<endl;
				if(put_into ==0)
				{
					//freelist[0]->get_head()->next = NULL;
					freelist[0]->insert(merged);
					break;
				}
				// else if(put_into == 1)
				// {
				else if(freelist[put_into]->get_head() == NULL)
					{
						freelist[put_into]->insert(merged);
						break;
					}
				// }

				freed_block = merged;
				buddy = getbuddy(freed_block);
				if(buddy == NULL || buddy->block_size != merged->block_size)
				{
					break;
				}
			}
		}
		else
		{
			if(buddy == NULL)
			{
				//do nothing
				return 0;
			}
			//something went wrong
			return 1;
		}

	}
  // delete _a;
  // return 0;
}

void BuddyAllocator::debug ()
{
cout<<"debug"<<endl;
int list_num= 0;
int l  = max_length;
while(list_num < freelist.size() && l >= min_length)
{
  BlockHeader * list_head = freelist[list_num]->get_head();
  if(list_head == NULL)
  {
    //cout<<"came here"<<endl;
    cout<<l<<" : "<<"0"<<endl;
  }
  else
  {
    int count = 0;
    while(list_head != NULL)
    {
      count++;
      list_head = list_head->next;
    }

    cout<<l<<" : "<<count<<endl;
  }

  l = l/2;
  list_num++;
}
}
