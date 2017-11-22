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
boost::mutex mutex_lock;
boost::shared_mutex mutex1;
boost::recursive_mutex rec_mutex;

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

vector<point> answer;

map<int,pair<int,int> > id_point_pair;

void put(node*head ,int x ,int y,int id){

	int bN = head->bucketNum;
	point p;
	p.x= x;
	p.y= y;
	p.id = id;
	buckets[bN].push_back(p);
	//cout<<bN<<" "<<x<<" "<<y<<endl;
}

void breakNode(node* head,int x,int y,int id){
	//boost::unique_lock<boost::shared_mutex> lock(mutex1);

  int bN = head->bucketNum;
  //cout<<"bucket number: "<<bN<<endl<<"breaking node:"<<endl;

  //boost::unique_lock<boost::shared_mutex> lock(mutex1);
  head->num++;
  put(head,x,y,id);
  //lock.unlock();
  if(head->flag == 0 ){

	  //cout<<"head flag is 0"<<endl;
	  //boost::shared_lock<boost::shared_mutex> lock1(mutex1);

  	int avgx =0;
  	for(int i=0;i<head->num;i++){
  		//avgx = (avgx*i + buckets[bN][i].x)/(i+1);
  		avgx +=buckets[bN][i].x;
  	}
  	avgx = avgx/head->num;
  	//cout<<"avgx is :"<<avgx<<endl;

  	//lock1.unlock();
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
  	//boost::unique_lock<boost::shared_mutex> lock(mutex1);

  	for(int i=0;i<head->num;i++){
  		//cout<<avgx<<endl;
  		if(buckets[bN][i].x <= avgx){

  			buckets[bN][head->left->num] = buckets[bN][i];
  			head->left->num++;
  		}else{
  			//cout<<buckets.size()-1<<endl;

  			buckets[buckets.size()-1].push_back(buckets[bN][i]);
  			head->right->num++;
  			//cout<<"here";
  		}
  	}

  	buckets[bN].resize(head->left->num);
  	//lock.unlock();
  }else{

	  //cout<<"head flag is 1"<<endl;
	  //boost::unique_lock<boost::shared_mutex> lock(mutex1);
	  //boost::shared_lock<boost::shared_mutex> lock1(mutex1);

  	int avgy =0;
  	for(int i=0;i<head->num;i++){
  		//avgy = (avgy*i + buckets[bN][i].y)/(i+1);
  		avgy +=buckets[bN][i].y;
  	}
  	avgy = avgy/head->num;

  	//cout<<"avgy is :"<<avgy<<endl;
  	//lock1.unlock();
  	head->cond = avgy;
  	head->left = new node();
  	head->right = new node();
  	head->left->bucketNum  = head->bucketNum;
  	head->right->bucketNum = buckets.size();
  	head->left->left = NULL;
	head->left->right  = NULL;
	head->right->left = NULL;
	head->right->right  = NULL;

	//cout<<"original bucket split into 2 left and right"<<endl;
  	vector<point> empty;
	buckets.push_back(empty);
  	head->left->num = 0;
  	head->right->num = 0;
  	head->left->flag = 1 - head->flag;
  	head->right->flag = 1 - head->flag;

  	//cout<<"setup flags for both the buckets"<<endl;
  	 //boost::unique_lock<boost::shared_mutex> lock(mutex1);

  	for(int i=0;i<head->num;i++){
  		//cout<<avgy<<endl;
  		if(buckets[bN][i].y <= avgy){

  			buckets[bN][head->left->num] = buckets[bN][i];
  			head->left->num++;
  		}else{
  			//cout<<buckets.size()-1<<endl;

  			buckets[buckets.size()-1].push_back(buckets[bN][i]);
  			head->right->num++;
  			//cout<<"here";
  		}
  	}

  	buckets[bN].resize(head->left->num);
  	//lock.unlock();
  }

  /*cout<<"breaking node output"<<endl;

  for(int m=0; m< buckets.size(); m++)
  		{
  			cout<<"bucket=======>>>>"<<m<<endl;
  			for(int n=0; n<buckets[m].size();n++)
  			{
  				cout<<"+++++++++++"<<buckets[m][n].x<<"++++======"<<buckets[m][n].y<<endl;
  			}
  		}*/
}

void insert(node* head,int x,int y,int id){
	//boost::lock_guard<boost::recursive_mutex> lock(rec_mutex);
	//if(head->num >= k){
		//boost::lock_guard<boost::recursive_mutex> lock(rec_mutex);
		/*if(head->left == NULL || head ->right == NULL){
			//cout<<head->num <<"calling breaknode"<<endl;
			breakNode(head,x,y,id);
			return;
		}*/
		//else{
			//boost::lock_guard<boost::recursive_mutex> lock(rec_mutex);
			//cout<<head->num <<"traversing:"<<endl;
			//cout<<head->flag<<endl;
			/*if(head->flag == 0){
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
			}*/
		//}
		//cout<<"he"<<endl;
		//node* test = NULL;
		//boost::lock_guard<boost::recursive_mutex> lock(rec_mutex);
	boost::unique_lock<boost::mutex> lock(mutex_lock);

	//boost::shared_lock<boost::mutex> lock1(mutex_lock);
		while(head->left!=NULL && head->right!=NULL){
			//cout<<"hello";

					/*if((head->flag == 0 && x <= head->cond) || (head->flag == 1  && y <= head->cond))
						head = head->left;
					else
						head = head->right;
					*/
					if(head->flag == 0){
							if(x <= head->cond){
								head = head->left;
							}else{
								head = head->right;
							}
						}else{
							if(y <= head->cond){
								head = head->left;
							}else{
								head = head->right;
							}
						}
		}
		//lock.unlock();

		//boost::unique_lock<boost::mutex> lock1(mutex_lock);
		//boost::unique_lock<boost::mutex> lock(mutex_lock);

		if(head->num >= k){

		breakNode(head,x,y,id);
		}
		else{
			//boost::lock_guard<boost::recursive_mutex> lock(rec_mutex);
			head->num = head->num+1;
			//lock.unlock();
					put(head,x,y,id);
		}



	/*}
	else{
		//boost::lock_guard<boost::recursive_mutex> lock(rec_mutex);
		std::ostringstream oss;
		oss << "bucket number:" <<head->bucketNum<<endl;
		oss << "head number :"<<head->num<<endl;
		oss <<	"head flag :"<<		head->flag<<endl;
		std::string idAsString = oss.str();
		//cout<<"normal insert=="<<endl<<idAsString<<endl;
		head->num = head->num+1;
		put(head,x,y,id);
	}*/

}

int query1(node* head,int x,int y, void (*fun)(int)){
	boost::shared_lock<boost::shared_mutex> lock(mutex1);
	//cout<<"+++++++++++"<<head->bucketNum<<endl;
	if(head->left == NULL || head ->right == NULL){
		//cout<<"+++++++++++"<<head->bucketNum<<endl;
			sleep(3);
			fun(head->bucketNum);

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

void addValidPoints(node* head,int x1,int y1,int x2,int y2)
{
	int bN = head->bucketNum;
	int size = buckets[bN].size();
	for(int i=0;i<size;i++){
  		//cout<<buckets[bN][i].x<<" "<<buckets[bN][i].y<<endl;
  		if(buckets[bN][i].x>=x1 && buckets[bN][i].x<x2 && buckets[bN][i].y>=y1 && buckets[bN][i].y<y2)
  		{
  			answer.push_back(buckets[bN][i]);
  		}
  	}
}

void query2(node* head,int x1,int y1,int x2,int y2){
	boost::shared_lock<boost::shared_mutex> lock(mutex1);
	if(head->left == NULL || head ->right == NULL){
		addValidPoints(head,x1,y1,x2,y2);
	}
	else{
		if(head->flag == 0){

			if(x1<=head->cond){
				 query2(head->left,x1,y1,x2,y2);
			}
			if(x2>head->cond){
			     query2(head->right,x1,y1,x2,y2);
			}

		}
		else{

			if(y1<=head->cond){
				 query2(head->left,x1,y1,x2,y2);
			}
			if(y2>head->cond){
			     query2(head->right,x1,y1,x2,y2);
			}
		}
	}

}

/*int sleep_print(int seconds) {
	std::cout << "going to sleep (" << seconds << ")" << std::endl;
	sleep(seconds);
	std::cout << "wake up (" << seconds << ")" << std::endl;
	return 0;
}*/

typedef boost::packaged_task<int> task_t;
typedef boost::shared_ptr<task_t> ptask_t;

void callbackFun(int x){
	std::ofstream out("output.txt", std::ofstream::out | std::ofstream::app);
	std::ostringstream oss;
	oss << "bucket number:" << x<<endl;


	//std::cout << oss.str();
	out << oss.str();


}

int increment_count(int seconds, void (*fun)(int))
{
	boost::shared_lock<boost::shared_mutex> lock(mutex1);
	std::cout << "count+++++"<<seconds<< std::endl;
	sleep(5);
	fun(seconds * 15);
	return 67;
}

void push_job(node* head, int x, int y, int i, boost::asio::io_service& ioService,
		std::vector<boost::shared_future<int> >& search_pending_data) {
	//boost::promise<std::string> promise;
	//boost::unique_future<std::string> fut = promise.get_future();
	//insert(head,x,y,i);
	/*ptask_t task = boost::make_shared<task_t>(boost::bind(&insert, head, x, y, i));

	boost::shared_future<int> fut(task->get_future());
	search_pending_data.push_back(fut);
	ioService.post(boost::bind(&task_t::operator(), task));*/
}

void query_bucket(node* head,int x,int y, boost::asio::io_service& ioService, std::vector<boost::shared_future<int> >& pending_data) {
	ptask_t task = boost::make_shared<task_t>(boost::bind(&query1,head, x,y, &callbackFun));
	boost::shared_future<int> fut(task->get_future());
	pending_data.push_back(fut);
	ioService.post(boost::bind(&task_t::operator(), task));
}


int main()
{
	id_point_pair.clear();
	buckets.clear();
	vector<point> empty;
	buckets.push_back(empty);
	// int numBucket=1;
	// map<int,pair<int,int> > map_init;
	// map_init.clear();
	// vec.push_back(map_init);

	boost::asio::io_service ioService;
	boost::thread_group reader_threads;
	boost::asio::io_service::work work(ioService);
	for (int i = 0; i < boost::thread::hardware_concurrency() ; ++i)
		{
			reader_threads.create_thread(boost::bind(static_cast<std::size_t(boost::asio::io_service::*)(void)>
				(&boost::asio::io_service::run), &ioService));
		}

	std::vector<boost::shared_future<int> > pending_data;


	boost::asio::io_service searchService;
		boost::thread_group search_threads;
		boost::asio::io_service::work searchWork(searchService);
		for (int i = 0; i < 8 ; ++i)
			{
			search_threads.create_thread(boost::bind(static_cast<std::size_t(boost::asio::io_service::*)(void)>
					(&boost::asio::io_service::run), &searchService));
			}

		std::vector<boost::shared_future<int> > search_pending_data;

	int max_num;
	node* head = new node();
	head->left = NULL;
	head->right  = NULL;
	head->num =0;
	head->flag = 1;
	head->bucketNum = 0;
	cout<<"Enter the max number of points in a bucket:"<<endl;
	cin>>max_num;
	k = max_num;
	ifstream fp;
	fp.open("test.txt");
	int i;
	int x;
	int y;
	boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::local_time();
	while(fp>>i>>x>>y)
	{
		searchService.post(boost::bind(&insert, head, x, y, i));
		//cout<<"head:"<<head<<"bucketNum:"<<head->bucketNum<<"number"<<head->num<<endl;
		//insert(head,x,y,i);
		id_point_pair[i]=make_pair(x,y);
	}
	//search_threads.join_all();
	//boost::wait_for_all(search_pending_data.begin(), search_pending_data.end());
	//searchService.stop();
	//

	while(buckets.size() != 2149){
		usleep(5);
	}
	boost::posix_time::ptime t2 = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration diff = t2 - t1;

	std::cout << "time take in ms(threads):"<<diff.total_microseconds() << std::endl;
		//searchService.stop();
		std::cout << "asdlfjal:"<<diff.total_nanoseconds() << std::endl;
		cout<<"------------"<<buckets.size()<<endl;
	cout<<"------------"<<buckets.size()<<endl;

	sleep(10);
	sleep(5);
	std::cout << "time take in ms(threads):"<<diff.total_microseconds() << std::endl;
		//searchService.stop();
		std::cout << "asdlfjal:"<<diff.total_nanoseconds() << std::endl;
		cout<<"------------"<<buckets.size()<<endl;

	/*for(int m=0; m< buckets.size(); m++)
		{
			cout<<"bucket=======>>>>"<<m<<endl;
			for(int n=0; n<buckets[m].size();n++)
			{
				cout<<"+++++++++++"<<buckets[m][n].x<<"++++======"<<buckets[m][n].y<<"id"<<buckets[m][n].id<<endl;
			}
		}
	*/



	/*while(1)
	{
		answer.clear();
		cout<<"Type 1 for search of point , Type 2 for search of all points in the rectangle , Anything else to exit :"<<endl;
		int val;
		cin>>val;
		if(val==1)
		{
			cout<<"Enter the point ID present in the database:"<<endl;
			int id;
			cin>>id;
			int x = id_point_pair[id].first;
			int y = id_point_pair[id].second;
			query_bucket(head, x, y, ioService, pending_data);

			//ioService.post(boost::bind(&query1, head, x, y));
			cout<<"input processed"<<endl;


			//cout<<"Bucket num: "<<query1(head,x,y)<<endl;
		}
		else if(val==2)
		{
			cout<<"Enter the xmin,ymin and xmax,ymax in which you need to find the points:";
			int xmin,ymin,xmax,ymax;
			cin>>xmin>>ymin>>xmax>>ymax;
			//ioService.post(boost::bind(&query2, head, xmin, ymin,xmax,ymax));
			query2(head,xmin,ymin,xmax,ymax);
			cout<<"Points included are:"<<endl;
			for(int i=0;i<answer.size();i++)
			{
				cout<<answer[i].id<<" "<<answer[i].x<<" "<<answer[i].y<<endl;
			}
		}
		else
		{
			return 0;
		}
	}*/

	return 0;
}

