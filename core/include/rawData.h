#ifndef __RAWDATA_H__
#define __RAWDATA_H__
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <stdlib.h>
#include <map>
#include <vector>
#include <unordered_map>
#include <cassert>

#include "KpixEvent.h"
/*
 Mengqing <mengqing.wu@desy.de>
 @ 2019-10-17
 @ finalized 2019-10-24
 Currently for external trigger only
 */

namespace Lycoris{

  constexpr uint G_BUCKET_HASH_UNIT { 100000 };
  
	class Cycle{
	public:
		// Global channel num = kpix_index*2014+channel;
		Cycle(KpixEvent &event,
		      uint nbuckets = 1,
		      uint begin_ch = 0,
		      uint end_ch   = 1023,
		      bool isold    = false);
		~Cycle(){};
		uint m_cyclenumber;
		uint16_t m_ts;
		uint m_nbuckets;
		// cm noise is per kpix, per bucket:
		std::unordered_map<uint, double> m_m_cm_noise;
		//double m_cm_noise[4];
		
		// Do your own boosted map
		vector<uint> m_v_hashkeys_b[4];
		const vector<uint>& hashkeys(uint i) const{
			if (i<4)return m_v_hashkeys_b[i];
			else {
				printf("hashkeys error\n");
				exit(EXIT_FAILURE);
			}
		}

		bool m_has_adc;
		// Please make sure both ADC and TS are 16bit (see KpixSample)
		std::vector<uint16_t> m_v_adc_b[4];
		const vector<uint16_t>& vadc(uint b) const{
			if (b<4) return m_v_adc_b[b];
			else {
				printf("vadc error\n");
				exit(EXIT_FAILURE);
			}
		}
		vector<uint16_t> m_v_ts_b[4];
		void ResetAdc(){
		  for (uint i=0; i<4; i++) m_v_adc_b[i].clear();
		  m_has_adc = false;
		  //printf("Reset ADC data for 1 cycle \n");
		}
		
		
		static void AddAdcBuf(Cycle&);
		static void ResetAdcBuf(){
		  s_buf_adc.clear();
		  printf("Static adc Buffer cleared.\n");
		}
		static void ResetPed(){
		  s_ped_adc.clear();
		  printf("[info] Static map Ped ADC cleared");
		}
		static void CalPed(uint nbuckets =1, bool save_mad = false);
		//	private:
		/* ADC data of all cycles: */
		static unordered_map<uint, vector<uint16_t>> s_buf_adc;
		/* Pedestal measured in a run: */
		static unordered_map<uint, uint> s_ped_adc;
		/* Pedestal MAD value: */
		static unordered_map<uint, uint> s_ped_mad;
		/* Channels with a MAD==0*/
		//		static std::vector<uint> s_v_mad0_chn;

		/* == Operation per cycle == */
		/* Remove ped in adc -> adc2Fc -> calculate common mode buffer*/
		void RemovePed_CalCM_fC(std::unordered_map<uint, uint > &ped_adc,
		                        std::unordered_map<uint, double> &slopes,
		                        std::unordered_map<uint, uint> &ped_mad,
		                        bool remove_adc,
		                        bool cut_mad0 = true,
		                        bool cut_slope0 = true);
		void RemoveCM();


		bool m_has_fc;

		// index by kpix/channel/bucket
		std::unordered_map<uint, double> m_m_fc_b;

		// index by kpix, vector of all channels' charge in fC
		std::unordered_map<uint, std::vector<double>> m_m_CM_buf;
				
		/* const vector<double>& vfc(uint b) const{ */
		/*   if (b<4) return m_v_fc_b[b]; */
		/*   else { */
		/*     printf("vadc error\n"); */
		/*     exit(EXIT_FAILURE); */
		/*   } */
		/* } */

	public:
		static void AddFcBuf(Cycle&);
		static void ResetFcBuf(){
		  s_buf_fc.clear();
		  printf("Fc buffer cleared.\n");
		}
		static void ResetNoise(){
		  s_noise_fc.clear();
		  printf("Noise [fC] cleared.\n");
		}
		static void CalNoise(uint& );
	private:
		/* over all cycles, indexed by channel/bucket */
		static unordered_map<uint, double> s_noise_fc;
		static unordered_map<uint, vector<double>> s_buf_fc;
	public:
		static unordered_map<uint, double>& getNoise() {
		  return Cycle::s_noise_fc;
		}

	public:
		
		// Hash table
		static uint hashCode(uint kpix, uint channel){ return kpix*1024+channel;}
		static uint hashCode(uint kpix, uint channel, uint bucket){return bucket*G_BUCKET_HASH_UNIT + kpix*1024+channel;}
		static uint getKpix(uint hashcode){ return hashcode/1024;}
		static uint getChannel(uint hashcode){ return hashcode%1024;}
		static uint rmBucket(uint hashcode) { return hashcode%G_BUCKET_HASH_UNIT;}
		static uint getBucket(uint hashcode){return hashcode/G_BUCKET_HASH_UNIT;}
		  

		template <typename T>
    	static void AddBufferT(std::unordered_map<uint, vector<T>> &target,
	                       //const std::vector<uint> &kk,
	                       //const std::vector<T> &vv,
	                       uint key,
	                       T val
	                       ){
			
			//		    for (size_t cc=0; cc < vv.size(); ++cc){
			/* auto key = Cycle::hashCode(getKpix(kk.at(cc)), */
			/* 			 getChannel(kk.at(cc)), */
			/* 			 bb); */
			/* auto val = vv.at(cc); */
			if (target.count(key))
				target.at(key).push_back(std::move(val));
			else{
				std::vector<T> vec;
				vec.push_back(val);
				target.insert(std::make_pair(key, std::move(vec)));
			}
			return;}	
	};
	
	class rawData{
	public:
	rawData(): m_nbuckets(1), m_nmax(0){};
	  ~rawData(){};

	  void loadFile(const std::string&);
	  void doRmPedCM(bool rmAdc=true); 
	  
	  void setNBuckets(uint level){
	    if (level<1 || level>4) return;
	    else m_nbuckets = level;
	  }
	  uint getNBuckets(){return m_nbuckets;}
	  
	  void setMaxCycles(uint nmax){
	    printf("[info] Set max cycles to load: %d\n",nmax);
	    m_nmax = nmax;
	  }
	  uint getNCycles(){return m_v_cycles.size();}
	  void loadCalibDB(const std::string&); // Done
	  //	  void loadCalib(const std::string&); 
	  const std::unordered_map<uint, double>& getSlopes() const{  return m_m_slopes_b0; }
	  const std::vector<Cycle>& getCycles() const{ return m_v_cycles;}
	private:
		std::vector<Cycle> m_v_cycles;
		uint m_nbuckets;
		uint m_nmax;
		std::unordered_map<uint, double> m_m_slopes_b0; //Done
			  
	};


    

}
#endif
