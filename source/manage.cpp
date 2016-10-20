#include "general.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/thread.hpp>

#include "http_server.hpp"

#include <execinfo.h>

/**
 * 包含管理维护线程的相关工作
 */

namespace airobot {


void backtrace_info(int sig, siginfo_t *info, void *f)
{
    int j, nptrs;
#define BT_SIZE 100
    char **strings;
    void *buffer[BT_SIZE];

    fprintf(stderr,       "\nSignal [%d] received.\n", sig);
    BOOST_LOG_T(fatal) << "\nSignal [" << sig << "] received.\n";
    fprintf(stderr,       "======== Stack trace ========");
    BOOST_LOG_T(fatal) << "======== Stack trace ========\n";

    nptrs = ::backtrace(buffer, BT_SIZE);
    BOOST_LOG_T(fatal) << "backtrace() returned %d addresses";
    fprintf(stderr,       "backtrace() returned %d addresses\n", nptrs);

    strings = ::backtrace_symbols(buffer, nptrs);
    if (strings == NULL)
    {
        perror("backtrace_symbols");
        BOOST_LOG_T(fatal) << "backtrace_symbols";
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
    {
        fprintf(stderr, "%s\n", strings[j]);
        BOOST_LOG_T(fatal) << strings[j];
    }

    free(strings);

    fprintf(stderr,       "Stack Done!\n");
    BOOST_LOG_T(fatal) << "Stack Done!";

    ::kill(getpid(), sig);
    ::abort();

#undef BT_SIZE
}

void backtrace_init()
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags     = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = backtrace_info;
    sigaction(SIGABRT, &act, NULL);
    sigaction(SIGBUS,  &act, NULL);
    sigaction(SIGFPE,  &act, NULL);
    sigaction(SIGSEGV, &act, NULL);

    return;
}

extern objects all_daemons;

void manage_thread(const objects* daemons)
{

    cerr<< "Manage ThreadID: " << boost::this_thread::get_id() << endl;
    http_server* p_srv = daemons->http_server_;

    for (;;)
    {
        boost::unique_lock<boost::mutex> notify_lock(p_srv->conn_notify_mutex);
        if(p_srv->conn_notify.timed_wait(notify_lock, boost::posix_time::seconds(45)))
        {
            assert(notify_lock.owns_lock());
            if(p_srv->pending_to_remove_.empty())
                continue;

            // 遍历，剔除失败的连接
            // 删除的时候，还是需要持有锁，因为新建连接的时候会创建插入元素，如果
            // 后面交换的话，会导致数据缺失
            // 这里算是个性能弱势点
            boost::lock_guard<boost::mutex> mutex_lock(p_srv->front_conns_mutex_);

            BOOST_LOG_T(info) << "Original connection: " << p_srv->front_conns_.size()
                                << ", trimed connection: " << p_srv->pending_to_remove_.size();

            // pending_to_remove可能在conn_stat以及co_worker timing
            // 两个地方同时被登记删除
            // 只有在最后一次删除的时候，才能进行洗洗白操作
            for (auto it=p_srv->pending_to_remove_.cbegin(); it!=p_srv->pending_to_remove_.cend(); it++)
            {
                front_conn_ptr item = *it;
                if(p_srv->front_conns_.find(item) != p_srv->front_conns_.end())
                {
                    // unordered_map, pending_to_remove_ 两份 + 此处的item局部变量一份
                    if (item.use_count() > 3)
                    {
                        // 这里需要重新加入time_wheel列表中，否则就永远漏检了
                        BOOST_LOG_T(info) << "SKIP with use count: " << item.use_count();
                        p_srv->timing_wheel_.back().insert(front_conn_weak(item));
                        continue;
                    }

                    p_srv->front_conns_.erase(item);
                    __sync_fetch_and_sub(&p_srv->current_conns_cnt_, 1);

                    // 无限制缓存连接数没有意义
                    if (p_srv->cached_conns_.size() < static_cast<size_t>(p_srv->max_serve_conns_cnt_ * 0.3) )
                    {
                        item->conn_wash_white();
                        p_srv->cached_conns_.push_back(item);
                    }
                }
            }

            p_srv->pending_to_remove_.clear(); // do really empty
            assert(p_srv->front_conns_.size() == p_srv->current_conns_cnt_);

            BOOST_LOG_T(info) << " Connection still alive: " << p_srv->current_conns_cnt_;

            continue; // skip show bellow if just wakend up!
        }

        //睡眠了30s，进行检查


        cout << "<<<<<" << to_simple_string(second_clock::local_time()) << ">>>>>" << endl; 
        p_srv->show_conns_info(false);
        cout << "====================" << endl;
    }
    return;
}



} // END NAMESPACE
