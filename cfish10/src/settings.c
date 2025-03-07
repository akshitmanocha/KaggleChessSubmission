#include "numa.h"
#include "search.h"
#include "settings.h"
#include "thread.h"
#include "tt.h"
#include "types.h"

struct settings settings, delayedSettings;

// Process Hash, Threads, NUMA and LargePages settings.

void process_delayed_settings(void)
{
  // print delayedttSize
  // printf("delayedSettings.ttSize: %ld\n", delayedSettings.ttSize);
  // print settings.ttSize
  // printf("settings.ttSize: %ld\n", settings.ttSize);

  bool ttChange = delayedSettings.ttSize != settings.ttSize;
  bool lpChange = delayedSettings.largePages != settings.largePages;
  bool numaChange =   (settings.numaEnabled != delayedSettings.numaEnabled)
                   || (   settings.numaEnabled
                       && !masks_equal(settings.mask, delayedSettings.mask));

#ifdef NUMA
  if (numaChange) {
    threads_set_number(0);
    settings.numThreads = 0;
#ifndef _WIN32
    if ((settings.numaEnabled = delayedSettings.numaEnabled))
      copy_bitmask_to_bitmask(delayedSettings.mask, settings.mask);
#endif
    settings.numaEnabled = delayedSettings.numaEnabled;
  }
#endif

//print delayedSettings.numThreads
  // printf("delayedSettings.numThreads: %ld\n", delayedSettings.numThreads);
  //print settings.numThreads
  // printf("settings.numThreads: %ld\n", settings.numThreads);

  if (settings.numThreads != delayedSettings.numThreads) {
    settings.numThreads = delayedSettings.numThreads;
    threads_set_number(settings.numThreads);
  }

  if (numaChange || ttChange || lpChange) {
    tt_free();
    settings.largePages = delayedSettings.largePages;
    settings.ttSize = delayedSettings.ttSize;
    // memory before tt allocate
    // print_memory_usage("tt_allocate before");
    tt_allocate(settings.ttSize);
    // memory after tt allocate
    // print_memory_usage("tt_allocate");
  }

  if (delayedSettings.clear) {
    delayedSettings.clear = false;
    // print_memory_usage("before clear");
    search_clear();
    // print_memory_usage("after clear");
  }
  // print_memory_usage("process_delayed_settings");
}
