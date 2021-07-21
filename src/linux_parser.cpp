#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stol;
using std::stoi;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float MemTotal, MemFree;
  int counter{0};
  string key, value, unit;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  while (counter<2) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> value >> unit;
    if (counter == 0) {
      MemTotal = stof(value);
    } else {
      MemFree = stof(value);
    }
    counter++;
  }
  return (MemTotal - MemFree)/MemTotal; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string TotTime;
  string line;
  long TotSec;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> TotTime;
    try {
      TotSec = stol(TotTime);
    } catch(...) {
      TotSec = 0;
    }
  }
  stream.close();
  return TotSec; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> values;
  string line, value;
  string kPidDirectory{std::to_string(pid)};
  kPidDirectory = "/" + kPidDirectory;
  std::ifstream stream(kProcDirectory + kPidDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  long utime_,stime_,cutime_,cstime_;
  try {
    utime_ = stol(values[13]);
  }catch(...){
    utime_ = 0;
  }
  try {
    stime_ = stol(values[14]);
  }catch(...){
    stime_ = 0;
  }
  try {
    cutime_ = stol(values[15]);
  }catch(...){
    cutime_ = 0;
  }
  try {
    cstime_ = stol(values[16]);
  }catch(...){
    cstime_ = 0;
  }
  stream.close();
  return (utime_ + stime_ + cutime_ + cstime_)/sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> CpuUtil = CpuUtilization();
  return stol(CpuUtil[CPUStates::kUser_]) + stol(CpuUtil[CPUStates::kNice_]) +
         stol(CpuUtil[CPUStates::kSystem_]) + stol(CpuUtil[CPUStates::kIRQ_]) +
         stol(CpuUtil[CPUStates::kSoftIRQ_]) +
         stol(CpuUtil[CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> CpuUtil = CpuUtilization();
  return stol(CpuUtil[CPUStates::kIdle_]) + stol(CpuUtil[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string cpu, line, value;
  vector<string> CpuUtil;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value) {
      CpuUtil.push_back(value);
    }
  }
  stream.close();
  return CpuUtil;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kProcesses) {
          return stoi(value);
        }
      }
    }
  }
  filestream.close();
  return stoi(value);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kProcessRunning) {
          return stoi(value);
        }
      }
    }
  }
  return stoi(value);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmd;
  string line;
  string kPidDirectory{std::to_string(pid)};
  kPidDirectory = "/" + kPidDirectory;
  std::ifstream stream(kProcDirectory + kPidDirectory + kCmdlineFilename);
  std::getline(stream, line);
  std::istringstream linestream(line);
  linestream >> cmd;
  return cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value{" "};
  string kPidDirectory{std::to_string(pid)};
  kPidDirectory = "/" + kPidDirectory;
  std::ifstream stream(kProcDirectory + kPidDirectory + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kMemKeyword) {
          return value = std::to_string(stol(value)/1024);
        }
      }
    }
  }
  stream.close();
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value;
  string kPidDirectory{std::to_string(pid)};
  kPidDirectory = "/" + kPidDirectory;
  std::ifstream stream(kProcDirectory + kPidDirectory + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kUidKeyword) {
          return value;
        }
      }
    }
  }
  stream.close();
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, key, value, user{}, uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> uid) {
        if (uid == Uid(pid)) {
          return key;
        }
      }
    }
  }
  filestream.close();
  return key;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  vector<string> values;
  long uptime;
  string line, value;
  string kPidDirectory{std::to_string(pid)};
  kPidDirectory = "/" + kPidDirectory;
  std::ifstream stream(kProcDirectory + kPidDirectory + kStatFilename);
  std::getline(stream, line);
  std::istringstream linestream(line);
  while (linestream >> value) {
    values.push_back(value);
  }
    uptime = stol(values[21])/sysconf(_SC_CLK_TCK);
  return uptime;
}
