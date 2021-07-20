#ifndef PROCESSOR_H
#define PROCESSOR_H


class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  void UpdateStatus(long active, long idle, long total);
  long GetIdle();
  long GetActive();
  long GetTotal();

  // TODO: Declare any necessary private members
 private:
  long idle_;
  long active_;
  long total_;

};

#endif