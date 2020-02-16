#include "experiments.h"
#include "options.h"
#include "series.h"
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// TODO: Some runs are incorrectly 0
// TODO: implement output_directory

static std::deque<Serie> series_queue;
static std::mutex series_mutex;
static std::mutex report_mutex;
static int total_nr_data_points = 0;
static int data_points_nr = 0;

void produce_one_data_point(std::string& algorithm, int key_size, double error_rate, int runs)
{
    std::this_thread::sleep_for(std::chrono::microseconds(20));

    {
        std::lock_guard<std::mutex> guard(report_mutex);
        data_points_nr += 1;
        std::cout << data_points_nr << "/" << total_nr_data_points
                  << " algorithm=" << algorithm
                  << " key_size=" << key_size
                  << " error_rate=" << error_rate
                  << " runs=" << runs << std::endl;
    }
}

void serie_worker()
{
    while (true) {

        // Get a serie from the queue for this worker to work on. Exit from the loop when the
        // queue is exhausted.
        Serie serie;
        {
            std::lock_guard<std::mutex> guard(series_mutex);
            if (series_queue.empty()) {
                break;
            }
            serie = series_queue.front();
            series_queue.pop_front();
        }

        // Produce every data point in the serie.
        for (auto key_size: serie.key_sizes) {
            for (auto error_rate: serie.error_rates) {
                produce_one_data_point(serie.algorithm, key_size, error_rate, serie.runs);
            }
        }

    }
}

void run_all_series(Series& series) {
    // Put all series on the queue. We don't need any locking because we have not started the
    // workers yet.
    for (Serie& serie: series.series) {
        series_queue.push_back(serie);
        total_nr_data_points += serie.key_sizes.size() * serie.error_rates.size();
    }

    // Start the workers.
    std::vector<std::thread> worker_threads;
    int nr_workers = std::thread::hardware_concurrency();
    for (int worker_nr = 0; worker_nr < nr_workers; ++worker_nr) {
        std::thread worker_thread = std::thread(serie_worker);
        worker_threads.push_back(std::move(worker_thread));
    }

    // Wait for all of the workers to complete.
    for (auto& worker_thread: worker_threads) {
        worker_thread.join();
    }

    // Consume all the results.
    // TODO
}

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);
    Experiments experiments(options.experiments_file);
    Series series(experiments, options.max_runs);
    run_all_series(series);
    return 0;
}
