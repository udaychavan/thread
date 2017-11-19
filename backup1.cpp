#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "boost/thread.hpp"
#include "boost/bind.hpp"
//#include "boost/asio.hpp"
#include "boost/move/move.hpp"
#include "iostream"
#include "unistd.h"
#include "boost/asio/io_service.hpp"
#include "boost/thread/thread.hpp"

using namespace std;

//namespace asio = boost::asio;

int sleep_print(int seconds) {
	std::cout << "going to sleep (" << seconds << ")" << std::endl;
	sleep(seconds);
	std::cout << "wake up (" << seconds << ")" << std::endl;
	return 0;
}


typedef boost::packaged_task<int> task_t;
typedef boost::shared_ptr<task_t> ptask_t;



void increment_count()
{
	std::cout << "count+++++"<< std::endl;
}

void push_job(int seconds, boost::asio::io_service& io_service, std::vector<boost::shared_future<int> >& pending_data) {
	ptask_t task = boost::make_shared<task_t>(boost::bind(&sleep_print, seconds));
	boost::shared_future<int> fut(task->get_future());
	pending_data.push_back(fut);
	io_service.post(boost::bind(&task_t::operator(), task));
}


int main() {

	boost::asio::io_service ioService;
	boost::thread_group threads;
	boost::asio::io_service::work work(ioService);
	ioService.post(&increment_count);
	threads.create_thread(boost::bind(static_cast<std::size_t(boost::asio::io_service::*)(void)>
	(&boost::asio::io_service::run), &ioService));

	//auto worker = boost::bind(&boost::asio::io_service::run, &io_service);
	//boost::thread* t1 = threads.create_thread(&increment_count);
	//boost::thread* t2 = threads.create_thread(&increment_count);
	//threads.add_thread(t2);

	/*cout<<"HELLO"<<threads.size()<<endl;
	for (int i = 0; i < boost::thread::hardware_concurrency() ; ++i)
	{
		threads.create_thread(&increment_count);
		//threads.add_thread(t);
		cout<<"========"<<i<<"+++++++"<<threads.size()<<endl;
		threads.join_all();

	}
	cout<<"HELLO"<<threads.size()<<endl;
	*/
	/*
	 *
	 *
	 * threads.create_thread(boost:bind(&functionPointer, arg1, arg2))
	 * threads.create_thread(boost:bind(&Class:function, &object));
	 *
	 * threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
		threads.create_thread(boost::bind(&boost::asio::io_service::run,
			&io_service));
	std::vector<boost::shared_future<int> > pending_data; // vector of futures

	sleep_print(2);
	push_job(3, io_service, pending_data);
	push_job(4, io_service, pending_data);


	boost::wait_for_all(pending_data.begin(), pending_data.end());

	push_job(3, io_service, pending_data);
	push_job(4, io_service, pending_data);
	push_job(5, io_service, pending_data);
	boost::wait_for_all(pending_data.begin(), pending_data.end());
	*/
	return 0;
}
