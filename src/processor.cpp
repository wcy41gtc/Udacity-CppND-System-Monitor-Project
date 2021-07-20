#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  long active = LinuxParser::ActiveJiffies();
  long idle = LinuxParser::IdleJiffies();
  long total = LinuxParser::Jiffies();
  long prev_total = GetTotal();
  long prev_idle = GetIdle();
  UpdateStatus(active, idle, total);
  float total_diff = float(total) - float(prev_total);
  float idle_diff = float(idle)-float(prev_idle);
  return (total_diff - idle_diff) / total_diff ;
}

long Processor::GetIdle(){
  return idle_;
}

long Processor::GetActive(){
  return active_;
}

long Processor::GetTotal(){
  return total_;
}

void Processor::UpdateStatus(long active, long idle, long total) {
  active_ = active;
  idle_ = idle;
  total_ = total;
}