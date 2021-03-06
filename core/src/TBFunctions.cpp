


#include "TBFunctions.h"


using namespace std;

std::bitset<18> keybit(uint kpix, uint channel, uint bucket){//}, uint time){
    uint test = bucket+std::pow(2,2)*kpix+std::pow(2,7)*channel;//+std::pow(2,17)*time;
    std::bitset<18> bittest(test);
    return bittest;
}

uint keyhash(uint kpix, uint channel, uint bucket){//}, uint time){
    return bucket+std::pow(2,2)*kpix+std::pow(2,7)*channel;//+std::pow(2,17)*time;
}
std::string bin(uint n, std::string output)
{
    /* step 1 */
    if (n > 1)
        output = output+bin(n/2, output);

    /* step 2 */
    output = output+ std::to_string(n % 2);
    return output;
}
char xor_c(char a, char b) { return (a == b) ? '0' : '1'; }

std::string binarytoGray(std::string binary){
    std::string gray = "";

    gray += binary[0];
    for (int i = 1; i < binary.length(); i++){
        gray  += xor_c(binary[i-1], binary[i]);
    }
    return gray;
}

double yParameter(double strip, int kpix)
{

	if (kpix%2 == 0) // if left kpix
	{
		if (kpix == 6 || kpix == 2 || kpix == 4) //if lower kpix
		{
			return 50*strip;
		}
		else
		{
			return 50*(-strip+1840);
		}
	}
	else  // if right kpix
	{
		if (kpix == 1 || kpix == 9 || kpix == 11) // if lower kpix
		{
			return 50*(-strip+1840);
		}
		else
		{
			return 50*strip;
		}
		
	}
}

double yParameterSensor(double strip, int sensor)
{
	if (sensor == 0 || sensor == 5 || sensor == 4) // kpix side showing towards beam movement beam side  KPIX >  < Beam
	{
		if (sensor == 5) return -50*(-strip+920); // sensor 2 and sensor 5 have no stereo angle
		else return -50*(-strip+920); // sensor 0,1, 3 and 4 have a 2 degree stereo angle means the y movement is slightly lower (50*cos(+-2/57.295))

	}
	else  // kpix side in direction of beam movement KPIX < < BEAM
	{
		if (sensor == 2) return -50*(strip-920); // sensor 2 and sensor 5 have no stereo angle
		else return -50*(strip-920); // sensor 0,1, 3 and 4 have a 2 degree stereo angle means the y movement is slightly lower (50*cos(+-2/57.295))
	}
}

double xParameterSensor(double strip, int sensor)
{
	if (sensor == 0 || sensor == 5 || sensor == 4) // kpix side showing towards beam movement beam side  KPIX >  < Beam
	{
		if (sensor == 5) return 0*(-strip+920); // sensor 2 and sensor 5 have no stereo angle
		else return -1.744998*(-strip+920); // sensor 0 and 4 have a -2 degree stereo angle means the y movement is slightly lower (50*sin(-2/57.295))

	}
	else  // kpix side in direction of beam movement KPIX < < BEAM
	{
		if (sensor == 2) return 0*(strip-920); // sensor 2 and sensor 5 have no stereo angle
		else return 1.744998*(strip-920); // sensor 1 and 3 have a 2 degree stereo angle means the y movement is slightly lower (50*sin(2/57.295))
	}
}

int sensor2layer(int sensor)
{
    switch (sensor) {
        case 0: return 10;
        case 1: return 11;
        case 2: return 12;
        case 3: return 15;
        case 4: return 14;
        case 5: return 13;
    }
}

// template <typename T>
// double median(std::vector<T>* v)
// {
//   /* Note by Mengqing: this func has already an optimal in memory/speed*/
// 	if (v  == nullptr )
// 	{
// 		//cout << "Found a nullpointer" << endl;
// 		return 0;
// 	}
// 	else
// 	{
// 		if (v->empty())
// 		{
// 			return 0;
// 		}
// 		else
// 		{
// 			size_t n = v->size() / 2;
// 			if (v->size()%2 == 0)
// 			{
// 				nth_element(v->begin(), v->begin()+n, v->end());
// 				nth_element(v->begin(), v->begin()+n-1, v->end());
// 				return (v->at(n)+v->at(n-1))/2;
// 			}
// 			else
// 			{
// 				nth_element(v->begin(), v->begin()+n, v->end());
// 				return v->at(n);
// 			}
// 		}
// 	}
// }

// double MAD(vector<double>* v)
// {
// 	if (v  == nullptr )
// 	{
// 		//cout << "Found a nullpointer" << endl;
// 		return 0;
// 	}
// 	else
// 	{
// 		if (v->empty())
// 		{
// 			return 0;
// 		}
// 		else
// 		{
// 			double med = median(v);
// 			vector<double>* deviation;
// 			deviation = new std::vector<double>;

// 			for (auto const i:(*v))
// 			{
// 				deviation->push_back(fabs(i - med));
// 			}
// 			return median(deviation);
// 		}
// 	}
// }
double smallest_time_diff( vector<double> ext_list, int int_value)
{
    double trigger_diff = 8200.0;
    for (uint k = 0; k<ext_list.size(); ++k)
    {
        double delta_t = int_value-ext_list.at(k);
        if (fabs(trigger_diff) > fabs(delta_t))
        {
            trigger_diff = delta_t;
        }
    }
    return trigger_diff;
}
