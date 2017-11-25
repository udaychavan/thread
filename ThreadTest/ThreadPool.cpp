#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "boost/thread.hpp"
#include "boost/bind.hpp"
#include "boost/move/move.hpp"
#include "iostream"
#include "unistd.h"
#include "boost/asio/io_service.hpp"
#include "boost/thread/thread.hpp"
#include <stdio.h>
#include <bits/stdc++.h>
#include <string>
#include "boost/thread/recursive_mutex.hpp"

using namespace std;
boost::mutex mutex_no,mutex_left;
boost::shared_mutex mutex1;
boost::recursive_mutex mutex_lock;
typedef boost::packaged_task<int> task_t;
typedef boost::shared_ptr<task_t> ptask_t;


typedef struct node
{
	struct node * left;
	struct node *right;
	int num;
	int flag;
	int cond;
	int bucketNum;
}node;

typedef struct point
{
	/* data */
	int x;
	int y;
	int id;

}point;

int k;

vector<vector<point> > buckets;

map<int,pair<int,int> > id_point_pair;



void put(node*head ,int x ,int y,int id){

	int bN = head->bucketNum;
	point p;
	p.x= x;
	p.y= y;
	p.id = id;
	buckets[bN].push_back(p);
}

void breakNode(node* head,int x,int y,int id){
  int bN = head->bucketNum;
  boost::unique_lock<boost::recursive_mutex> lock(mutex_lock);
  head->num++;
  put(head,x,y,id);
  lock.unlock();

  if(head->flag == 0 ){
  	int avgx =0;
  	for(int i=0;i<head->num;i++){
  		//avgx = (avgx*i + buckets[bN][i].x)/(i+1);
  		avgx +=buckets[bN][i].x;
  	}
  	avgx = avgx/head->num;
  	lock.lock();
  	head->cond = avgx;
  	head->left = new node();
  	head->right = new node();
  	head->left->bucketNum  = head->bucketNum;
  	head->right->bucketNum = buckets.size();
  	vector<point> empty;
	buckets.push_back(empty);
  	head->left->num = 0;
  	head->left->left = NULL;
	head->left->right  = NULL;
	head->right->left = NULL;
	head->right->right  = NULL;
  	head->right->num = 0;
  	head->left->flag = 1 - head->flag;
  	head->right->flag = 1 - head->flag;
  	for(int i=0;i<head->num;i++){
  		if(buckets[bN][i].x <= avgx){
  			buckets[bN][head->left->num] = buckets[bN][i];
  			head->left->num++;
  		}else{
  			buckets[buckets.size()-1].push_back(buckets[bN][i]);
  			head->right->num++;
  		}
  	}

  	buckets[bN].resize(head->left->num);
  	lock.unlock();
  }else{
  	int avgy =0;
  	for(int i=0;i<head->num;i++){
  		//avgy = (avgy*i + buckets[bN][i].y)/(i+1);
  		avgy +=buckets[bN][i].y;
  	}
  	avgy = avgy/head->num;
  	lock.lock();
  	head->cond = avgy;
  	head->left = new node();
  	head->right = new node();
  	head->left->bucketNum  = head->bucketNum;
  	head->right->bucketNum = buckets.size();
  	head->left->left = NULL;
	head->left->right  = NULL;
	head->right->left = NULL;
	head->right->right  = NULL;
  	vector<point> empty;
	buckets.push_back(empty);
  	head->left->num = 0;
  	head->right->num = 0;
  	head->left->flag = 1 - head->flag;
  	head->right->flag = 1 - head->flag;
  	for(int i=0;i<head->num;i++){
  		if(buckets[bN][i].y <= avgy){
  			buckets[bN][head->left->num] = buckets[bN][i];
  			head->left->num++;
  		}else{
  			buckets[buckets.size()-1].push_back(buckets[bN][i]);
  			head->right->num++;
  		}
  	}

  	buckets[bN].resize(head->left->num);
  	lock.unlock();
  }
}

void insert(node* head,int x,int y,int id){
	boost::shared_lock<boost::shared_mutex> readlock(mutex1);
	if(head->num >= k){
		if(head->left == NULL){
			breakNode(head,x,y,id);
			return;
		}
		else{
			if(head->flag == 0){
				if(x <= head->cond){
					insert(head->left,x,y,id);
				}else{
					insert(head->right,x,y,id);
				}
			}else{
				if(y <= head->cond){
					insert(head->left,x,y,id);
				}else{
					insert(head->right,x,y,id);
				}
			}
		}
	}
	else{

		boost::unique_lock<boost::recursive_mutex> lock(mutex_lock);
		head->num = head->num+1;
		put(head,x,y,id);
	}
}

int query1(node* head,int x,int y, void (*fun)(int) = NULL){
	boost::shared_lock<boost::shared_mutex> readlock(mutex1);
	if(head->left == NULL || head ->right == NULL){
			return head->bucketNum;
	}
	if(head->flag == 0){
		if(x<=head->cond){
			return query1(head->left,x,y,fun);
		}else{
		    return query1(head->right,x,y, fun);
		}

	}
	else{
		if(y<=head->cond){
			return query1(head->left,x,y, fun);
		}else{
		    return query1(head->right,x,y, fun);
		}
	}
}

void addValidPoints(node* head,int x1,int y1,int x2,int y2, vector<point>* answer)
{
	int bN = head->bucketNum;
	int size = buckets[bN].size();
	for(int i=0;i<size;i++){
  		//cout<<buckets[bN][i].x<<" "<<buckets[bN][i].y<<endl;
  		if(buckets[bN][i].x>=x1 && buckets[bN][i].x<x2 && buckets[bN][i].y>=y1 && buckets[bN][i].y<y2)
  		{
  			answer->push_back(buckets[bN][i]);
  		}
  	}
}

void query2(node* head,int x1,int y1,int x2,int y2, vector<point>* answer){
	boost::shared_lock<boost::shared_mutex> readlock(mutex1);
	if(head->left == NULL || head ->right == NULL){
		addValidPoints(head,x1,y1,x2,y2, answer);
	}
	else{
		if(head->flag == 0){

			if(x1<=head->cond){
				 query2(head->left,x1,y1,x2,y2, answer);
			}
			if(x2>head->cond){
			     query2(head->right,x1,y1,x2,y2, answer);
			}

		}
		else{

			if(y1<=head->cond){
				 query2(head->left,x1,y1,x2,y2, answer);
			}
			if(y2>head->cond){
			     query2(head->right,x1,y1,x2,y2, answer);
			}
		}
	}

}

int findPointsInRange(node* head, int xmin, int ymin, int xmax, int ymax){

	vector<point>* answer = new vector<point>();
	query2(head,xmin,ymin,xmax,ymax, answer);
	std::stringstream ss;
	ss << "range_output/output_" << xmin <<"_"<< ymin <<"_"<< xmax <<"_"<< ymax <<"_"<< ".txt";
	std::string filename = ss.str();

	std::ofstream out(filename, std::ofstream::out | std::ofstream::app);
	std::ostringstream oss;
	oss << "points in the range are:"<<endl;
	vector<point>::iterator it;
		for(it = answer->begin() ; it != answer->end() ; ++it)
		{
			//cout<<it->id<<" "<<it->x<<" "<<it->y<<endl;
			oss <<it->id<<" "<<it->x<<" "<<it->y<<endl;
		}
	out << oss.str();
	return 1;

}

int findBucketOfPoint(node* head,int x,int y){
	std::stringstream ss;
	ss << "Bucket num for (" << x <<", "<< y <<") :";
	std::string s = ss.str();
	std::ofstream out("output.txt", std::ofstream::out | std::ofstream::app);
	std::ostringstream oss;
	oss<<s<<query1(head,x,y)<<endl;
	out<< oss.str();
	return 0;
}

void postPointQuery(node* head, boost::asio::io_service& searchService,
		std::vector<boost::shared_future<int> >& search_pending_data){

	//reading point ids from file and quering
	ifstream fp;
	fp.open("pointQuery.txt");
	int x, y, id;
	while(fp>>id)
	{
		x = id_point_pair[id].first;
		y = id_point_pair[id].second;
		ptask_t task = boost::make_shared<task_t>(boost::bind(&findBucketOfPoint, head, x, y));
		boost::shared_future<int> fut(task->get_future());
		search_pending_data.push_back(fut);
		searchService.post(boost::bind(&task_t::operator(), task));
	}

}

void postRangeQuery(node* head, boost::asio::io_service& searchService,
		std::vector<boost::shared_future<int> >& search_pending_data){

		ifstream fp;
		fp.open("rangeQuery.txt");
		int xmin, ymin, xmax, ymax;
		while(fp>>xmin>>ymin>>xmax>>ymax)
		{
			ptask_t task = boost::make_shared<task_t>(boost::bind(&findPointsInRange, head, xmin,ymin,xmax,ymax));
			boost::shared_future<int> fut(task->get_future());
			search_pending_data.push_back(fut);
			searchService.post(boost::bind(&task_t::operator(), task));
		}

}


void readInput(node * head){
	//thread pool for inserting the points
	boost::asio::io_service readerService;
	boost::thread_group reader_threads;
	boost::asio::io_service::work work(readerService);
	for (int i = 0; i < boost::thread::hardware_concurrency() ; ++i)
		{
			reader_threads.create_thread(boost::bind(static_cast<std::size_t(boost::asio::io_service::*)(void)>
				(&boost::asio::io_service::run), &readerService));
		}

	std::vector<boost::shared_future<int> > pending_data;

	head->left = NULL;
	head->right  = NULL;
	head->num =0;
	head->flag = 1;
	head->bucketNum = 0;
	//cout<<"Enter the max number of points in a bucket:"<<endl;
	//cin>>max_num;
	//if you want we can read this from file
	k = 20;
	cout<<endl<<"bucket size is set to: "<<k<<endl;

	ifstream fp;
	fp.open("input.txt");
	int i;
	int x;
	int y;
	while(fp>>i>>x>>y)
	{
		readerService.post(boost::bind(&insert, head, x, y, i));
		//usleep(50);
		id_point_pair[i]=make_pair(x,y);
	}
	cout<<"reading input points from file is complete. waiting for all insert operations to complete."<<endl;

	reader_threads.join_all();
}
boost::asio::io_service searchService;
std::vector<boost::shared_future<int> > search_pending_data;

void searchPointBucket(node* head){
	postPointQuery(head, searchService, search_pending_data);
	cout<<"reading point queries complete. Results are being fetched and written to output file"<<endl;
}

void searchPointsInRange(node* head){
	postRangeQuery(head, searchService, search_pending_data);
	cout<<"reading range queries complete. Results are being fetched and written to output directory"<<endl;
}

int main()
{
	id_point_pair.clear();
	buckets.clear();
	vector<point> empty;
	buckets.push_back(empty);

	node* head = new node();
	head->left = NULL;
	head->right  = NULL;
	head->num =0;
	head->flag = 1;
	head->bucketNum = 0;

	//one thread for each type of operation - insert, point query, range query
	boost::thread readThread(boost::bind(&readInput, head));
	boost::thread* pointSearch;
	boost::thread* rangeSearch;

	//thread pool for search operations
	boost::thread_group search_threads;
	boost::asio::io_service::work work(searchService);
	for (int i = 0; i < boost::thread::hardware_concurrency() ; ++i)
	{
		search_threads.create_thread(boost::bind(static_cast<std::size_t(boost::asio::io_service::*)(void)>
			(&boost::asio::io_service::run), &searchService));
	}

	while(1)
	{
		cout<<"Type 1 for search of point , Type 2 for search of all points in the rectangle, Type 3 to check buckets , Anything else to exit :"<<endl;
		int val;
		cin>>val;
		if(val==1)
		{
			pointSearch = new boost::thread(boost::bind(&searchPointBucket, head));
		}
		else if(val==2)
		{
			rangeSearch = new boost::thread(boost::bind(&searchPointsInRange, head));
			//postRangeQuery(head, searchService, search_pending_data);
		}
		else if(val==3)
		{
			cout<<"number of buckets created so far ==:"<<buckets.size()<<endl;
		}
		else
		{
			cout<<"you have opted to exit???"<<endl;
			return 0;
		}
	}
	readThread.join();
	pointSearch->join();
	rangeSearch->join();
	return 0;
}

