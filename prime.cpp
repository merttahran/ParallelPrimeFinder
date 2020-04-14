#include <omp.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>

int main (int argc, char *argv[]){
  std::string tempM = std::string(argv[1]);
  //upper bound assignment
  long long M = stoll(tempM);
  /* if the number is less than 3, there is no proper calculation
  as a result I ignore them as input.
  */
  if(M<=2){
    return M-1;
  }
  //chunk size assignment
  std::string tempChunk = std::string(argv[2]);
  long long Chunk = stoll(tempChunk);
  //boundary calculation
  long long boundary = (long long)floor(sqrt(M));
  double static_time[4], dynamic_time[4], guided_time[4];
  std::vector<long long> primes;
  primes.push_back(2);
  omp_lock_t mutex;
  omp_init_lock(&mutex);
  double start_common = omp_get_wtime();
  /*find prime numbers less than sqrt(N) sequentially
  also this part find the least prime number greater than sqrt(N)
  */
  for(int n = 3; n <= 2*boundary+1; n+=2){
    for(int k=0; k<primes.size(); k++){
      if(n % primes[k] == 0){
        break;
      }
      if(n/primes[k]<=primes[k]){
        primes.push_back(n);
        if(n>boundary){
          n=2*(boundary+1);
        }
        break;
      }
    }
  }
  double end_common = omp_get_wtime();
  int last_prime = primes.back();
  boundary = last_prime+2;
  double seq_time = end_common-start_common;//sequential time
  float my_length = primes.size();
  long long n,k;
  std::vector<long long> new_primes;

  for(int i = 1; i < 4; i++) {
    if(i==1){
      omp_set_schedule(omp_sched_static,Chunk);
    }else if(i==2){
      omp_set_schedule(omp_sched_dynamic,Chunk);
    }else{
      omp_set_schedule(omp_sched_guided,Chunk);
    }
    double beginning, parallel_end;
    for(int j = 1,k=0; j <= 8; j*=2,k++) {
      omp_set_num_threads(j);
      new_primes.clear();
      beginning = omp_get_wtime();
      #pragma omp parallel for shared(primes,boundary,new_primes,i) private(k)
      for(n = boundary; n <= M; n+=2){
        for(k=0; k<primes.size(); k++){
          if(n % primes[k] == 0){
            break;
          }
          if(n/primes[k]<=primes[k]){
            omp_set_lock(&mutex);
            new_primes.push_back(n);
            omp_unset_lock(&mutex);
            break;
          }
        }
      }
      parallel_end = omp_get_wtime();
      if(i==1){
        static_time[k]=parallel_end-beginning+seq_time;
      }else if(i==2){
        dynamic_time[k]=parallel_end-beginning+seq_time;
      }else{
        guided_time[k]=parallel_end-beginning+seq_time;
      }
    }
  }
  omp_destroy_lock(&mutex);
  my_length = new_primes.size();
  double sizeoflittles = primes.size();
  char buffer[73];
  char fileheader[]="M, Openmp Loop Scheduling Method, Chunk Size, T1, T2, T4, T8, S2, S4, S8\n";
  /*
    if format of results.csv is not appropriate to desired file format,
     overwrite the file in desired format
  */
  FILE * result=fopen("results.csv", "r");
  if (result != NULL){
    fgets (buffer , 74 , result);
    if(strcmp(fileheader,buffer)!=0){
      freopen ("results.csv","w",result);
      fputs(fileheader,result);
    }
  }else{
    result=fopen ("results.csv","w");
    fputs(fileheader,result);
  }

  freopen ("results.csv","a",result);
  //print results to results.csv file
  fprintf(result,"%lld, %s, %lld, %f, %f, %f, %f, %f, %f, %f\n",M,"static", Chunk,static_time[0], static_time[1], static_time[2], static_time[3], static_time[0]/static_time[1], static_time[0]/static_time[2], static_time[0]/static_time[3]);
  fprintf(result,"%lld, %s, %lld, %f, %f, %f, %f, %f, %f, %f\n",M,"dynamic", Chunk,dynamic_time[0], dynamic_time[1], dynamic_time[2], dynamic_time[3], dynamic_time[0]/dynamic_time[1], dynamic_time[0]/dynamic_time[2], dynamic_time[0]/dynamic_time[3]);
  fprintf(result,"%lld, %s, %lld, %f, %f, %f, %f, %f, %f, %f\n",M,"guided", Chunk,guided_time[0], guided_time[1], guided_time[2], guided_time[3], guided_time[0]/guided_time[1], guided_time[0]/guided_time[2], guided_time[0]/guided_time[3]);
  fclose(result);
  // print prime numbers to console
  if(argv[3]!= NULL &&  strcmp(argv[3],"--printnumbers")==0){
    for (long long pr: primes ){
      printf("%lld, ",pr );
    }
    std::sort(new_primes.begin(),new_primes.end());
    for (long long pr: new_primes ) {
      printf("%lld, ",pr );
    }
    printf("\n");
  }
}
