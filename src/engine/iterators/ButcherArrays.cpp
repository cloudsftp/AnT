/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * This file is part of AnT,
 * a simulation and analysis tool for dynamical systems.
 * 
 * AnT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AnT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id: ButcherArrays.cpp,v 1.3 2004/04/06 15:11:57 schanzml Exp $
 *
 */

#include "ButcherArrays.hpp"
#include "../utils/config/Configuration.hpp"

int ButcherArrays::get (Configuration& integrationMethodDescription,
			Array<real_t> &nominatorsArray,
			Array<real_t> &denominatorsArray) 
{
  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","EULER_FORWARD_KEY") )
    {    
      nominatorsArray.alloc(1);
      denominatorsArray.alloc(2);
      
      nominatorsArray[0] = 1.0;
      denominatorsArray[0] = 1.0;
      denominatorsArray[1] = 1.0;
      // 1 stage formula of order (1)      
	
      return 1;
    } 

  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","EULER_BACKWARD_KEY") )
    {    
      nominatorsArray.alloc(3);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 1.0;
      nominatorsArray[1] = 0.0;
      nominatorsArray[2] = 1.0;
      denominatorsArray[0] = 1.0;
      denominatorsArray[1] = 1.0;
      // 2 stage formula of order (1)      

      return 1;

    } 

  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","MIDPOINT_KEY") ) 
    {
      nominatorsArray.alloc(3);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 1.0;
      denominatorsArray[0] = 2.0;

      nominatorsArray[1] = 0.0;
      nominatorsArray[2] = 1.0;
      denominatorsArray[1] = 1.0;
      // 2 stage formula of order (2)

      return 2;

    } 

  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","HEUN_KEY") ) 
    {
      nominatorsArray.alloc(3);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 2.0;
      denominatorsArray[0] = 2.0;

      nominatorsArray[1] = 1.0;
      nominatorsArray[2] = 1.0;
      denominatorsArray[1] = 2.0;
      // 2 stage formula of order (2)

      return 2;

    } 


  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","RALSTON_KEY") ) 
    {
      nominatorsArray.alloc(6);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 2.0;
      nominatorsArray[1] = 0.0;
      nominatorsArray[2] = 3.0;
      denominatorsArray[0] = 4.0;

      nominatorsArray[3] = 2.0;
      nominatorsArray[4] = 3.0;
      nominatorsArray[5] = 4.0;
      denominatorsArray[1] = 9.0;
      // 3 stage formula of order (3)

      return 3;
    } 


  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","RADAU_KEY") ) 
    {
      nominatorsArray.alloc(6);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 2.0;
      nominatorsArray[1] = 1.0;
      nominatorsArray[2] = 1.0;
      denominatorsArray[0] = 3.0;

      nominatorsArray[3] = 1.0;
      nominatorsArray[4] = 0.0;
      nominatorsArray[5] = 3.0;
      denominatorsArray[1] = 4.0;
      // 3 stage formula of order (3)

      return 3;

    } 


  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","RK44_KEY") ) 

    {

      nominatorsArray.alloc(10);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 1.0;
      nominatorsArray[1] = 0.0;
      nominatorsArray[2] = 1.0;
      nominatorsArray[3] = 0.0;
      nominatorsArray[4] = 0.0;
      nominatorsArray[5] = 2.0;
      denominatorsArray[0] = 2.0;

      nominatorsArray[6] = 1.0;
      nominatorsArray[7] = 2.0;
      nominatorsArray[8] = 2.0;
      nominatorsArray[9] = 1.0;
      denominatorsArray[1] = 6.0;
      // 4 stage formula of order 4

	return 4;
    }  

  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","GILL44_KEY") ) 

    {

      nominatorsArray.alloc(10);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 1.0;
      nominatorsArray[1] = -1.0+sqrt(2.0);
      nominatorsArray[2] = 2.0-sqrt(2.0);
      nominatorsArray[3] = 0.0;
      nominatorsArray[4] = -sqrt(2.0);
      nominatorsArray[5] = 2.0+sqrt(2.0);
      denominatorsArray[0] = 2.0;

      nominatorsArray[6] = 1.0;
      nominatorsArray[7] = 2.0-sqrt(2.0);
      nominatorsArray[8] = 2.0+sqrt(2.0);
      nominatorsArray[9] = 1.0;
      denominatorsArray[1] = 6.0;
      // 4 stage formula of order 4

	return 4;
    }  



    if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","RKM45_KEY") ) 
    {
	nominatorsArray.alloc(15);
	denominatorsArray.alloc(2);

	nominatorsArray[0] = 8.0;
	nominatorsArray[1] = 4.0;
	nominatorsArray[2] = 4.0;
	nominatorsArray[3] = 3.0;
	nominatorsArray[4] = 0.0;
	nominatorsArray[5] = 9.0;
	nominatorsArray[6] = 12.0;
	nominatorsArray[7] = 0.0;
	nominatorsArray[8] = -36.0;
	nominatorsArray[9] = 48.0;
	denominatorsArray[0] = 24.0;

	nominatorsArray[10] = 1.0;
	nominatorsArray[11] = 0.0;
	nominatorsArray[12] = 0.0;
	nominatorsArray[13] = 4.0;
	nominatorsArray[14] = 1.0;
	denominatorsArray[1] = 6.0;
	// 5 stage formula of order 4

	return 4;
    } 

    if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY", "RKF456_KEY") ) 
{
	nominatorsArray.alloc(21);
	denominatorsArray.alloc(2);

	nominatorsArray[0] = 45082440.0;              //1.0/4.0;
	nominatorsArray[1] = 16905915.0;              //3.0/32.0;
	nominatorsArray[2] = 50717745.0;              //9.0/32.0;
	nominatorsArray[3] = 158578560.0;             //1932.0/2197.0;
	nominatorsArray[4] = -590976000.0;            //-7200.0/2197.0;
	nominatorsArray[5] = 598855680.0;             //7296.0/2197.0;
	nominatorsArray[6] = 366503540.0;             //439.0/216.0;
	nominatorsArray[7] = -1442638080.0;           //-8.0;
	nominatorsArray[8] = 129359600.0;             //3680.0/513.0;
	nominatorsArray[9] = -37129300.0;             //-845.0/4104.0;
	nominatorsArray[10] = -53431040.0;            //-8.0/27.0;
	nominatorsArray[11] = 360659520.0;            //2.0;
	nominatorsArray[12] = -249157376.0;           //-3544.0/2565.0;
	nominatorsArray[13] = 81684460.0;             //1859.0/4104.0;
	nominatorsArray[14] = -49590684.0;            //-11.0/40.0;
	denominatorsArray[0] = 180329760.0;

	nominatorsArray[15] = 33440.0;
	nominatorsArray[16] = 0.0;
	nominatorsArray[17] = 146432.0;
	nominatorsArray[18] = 142805.0;
	nominatorsArray[19] = -50787;
	nominatorsArray[20] = 10260;
	denominatorsArray[1] = 282150.0;
	// 6 stage formula of order (4,5)

	return 4;

        }


  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","VERN568_KEY") ) 
    {


      nominatorsArray.alloc(36);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 84180096.0;                  //1.0/18.0;
      nominatorsArray[1] = -126270144.0;                //-1.0/12.0;
      nominatorsArray[2] = 378810432.0;                 //1.0/4.0;
      nominatorsArray[3] = -37413376.0;                 //-2.0/81.0;
      nominatorsArray[4] = 224480256.0;                 //4.0/27.0;
      nominatorsArray[5] = 149653504.0;                 //8.0/81.0;
      nominatorsArray[6] = 1836656640.0;                //40.0/33.0;
      nominatorsArray[7] = -550996992.0;                //-4.0/11.0;
      nominatorsArray[8] = -7713957888.0;               //-56.0/11.0;
      nominatorsArray[9] = 7438459392.0;                //54.0/11.0;
      nominatorsArray[10] = -7659235584.0;              //-369.0/73.0;
      nominatorsArray[11] = 1494484992.0;               //72.0/73.0;
      nominatorsArray[12] = 37223746560.0;              //5380.0/219.0;
      nominatorsArray[13] = -31874562720.0;             //-12285.0/584.0;
      nominatorsArray[14] = 2330808480.0;               //2695.0/1752.0;
      nominatorsArray[15] = -14822499328.0;             //-8716.0/891.0;
      nominatorsArray[16] = 3346796544.0;               //656.0/297.0;
      nominatorsArray[17] = 67208028160.0;              //39520.0/891.0;
      nominatorsArray[18] = -57303687168.0;             //-416.0/11.0;
      nominatorsArray[19] = 2918243328.0;               //52.0/27.0;
      nominatorsArray[20] = 0.0;
      nominatorsArray[21] = 17845522695.0;              //3015.0/256.0;
      nominatorsArray[22] = -3409293888.0;              //-9.0/4.0;
      nominatorsArray[23] = -81959036544.0;             //-4219.0/78.0;
      nominatorsArray[24] = 70849388610.0;              //5985.0/128.0;
      nominatorsArray[25] = -2126862738.0;              //-539.0/384.0;
      nominatorsArray[26] = 0.0;
      nominatorsArray[27] = 315523593.0;                //693.0/3328.0;
      denominatorsArray[0] = 1515241728.0;

      nominatorsArray[28] = 5187.0;                     //57.0/640.0;
      nominatorsArray[29] = 0.0;
      nominatorsArray[30] = -14336.0;                   //-16.0/65.0;
      nominatorsArray[31] = 35802.0;                    //1377.0/2240.0;
      nominatorsArray[32] = 22022.0;                    //121.0/320.0;
      nominatorsArray[33] = 0.0;
      nominatorsArray[34] = 6237.0;                     //891.0/8320.0;
      nominatorsArray[35] = 3328;                       //2.0/35.0;
      denominatorsArray[1] = 58240.0;
      // 8 stage formula of order (5,6)

      return 5;
    } 

  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY", "VERN6710_KEY") ) 
    {


      nominatorsArray.alloc(55);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 711934597248075460759500.0; // 1.0/18.0
      nominatorsArray[1] = 0.0; // 0.0/1
      nominatorsArray[2] = 1423869194496150921519000.0; // 1.0/9.0
      nominatorsArray[3] = 533950947936056595569625.0; // 1.0/24.0
      nominatorsArray[4] = 0.0; // 0.0/1
      nominatorsArray[5] = 1601852843808169786708875.0; // 1.0/8.0
      nominatorsArray[6] = 6961138284203404505204000.0; // 44.0/81.0
      nominatorsArray[7] = 0.0; // 0.0/1
      nominatorsArray[8] = -26578891630594817201688000.0; // -56.0/27.0
      nominatorsArray[9] = 25313230124376016382560000.0; // 160.0/81.0
      nominatorsArray[10] = 1711742682118037811944625.0; // 91561.0/685464.0
      nominatorsArray[11] = 0.0; // 0.0/1
      nominatorsArray[12] = -5387780245355135408088000.0; // -12008.0/28561.0
      nominatorsArray[13] = 8240803117895513018700000.0; // 55100.0/85683.0
      nominatorsArray[14] = 1678353221209323233334375.0; // 29925/228488.0
      nominatorsArray[15] = -18225255265686116059286875.0; // -1873585.0/1317384.0
      nominatorsArray[16] = 0.0; // 0.0/1
      nominatorsArray[17] = 69552239780995783331520000.0; // 15680.0/2889.0
      nominatorsArray[18] = -47350833641760115026463904.0; // -4003076.0/1083375.0
      nominatorsArray[19] = -26236253699352277706570445.0; // -43813.0/21400.0
      nominatorsArray[20] = 32227187187275781911434224.0; // 5751746.0/2287125.0
      nominatorsArray[21] = 50919790427079870799360000.0; // 50383360.0/12679821.0
      nominatorsArray[22] = 0.0; // 0.0/1
      nominatorsArray[23] = -174945174551178169298160000.0; // -39440.0/2889.0
      nominatorsArray[24] = 123021714989941363014368768.0; // 1258442432.0/131088375.0
      nominatorsArray[25] = 97062537843388796384946240.0; // 222872.0/29425.0
      nominatorsArray[26] = -91918285966545366603283008.0; // -9203268152.0/1283077125.0
      nominatorsArray[27] = 7250370813282713074920000.0; // 24440.0/43197.0
      nominatorsArray[28] = -46464372996640655934574125.0; // -22942833.0/6327608.0
      nominatorsArray[29] = 0.0; // 0.0/1
      nominatorsArray[30] = 154682905047823319279112000.0; // 71784.0/5947.0
      nominatorsArray[31] = -94975322264123358837780000.0; // -572980.0/77311.0
      nominatorsArray[32] = -119767253696835993746623125.0; // -444645.0/47576.0
      nominatorsArray[33] = 120195956190270307977630000.0; // 846789710.0/90281407.0
      nominatorsArray[34] = -4359473072609924090250000.0; // -240750.0/707693.0
      nominatorsArray[35] = 3502383542581663646156250.0; // 3972375.0/14534468.0
      nominatorsArray[36] = 60130137535910219189894625.0; // 3379947.0/720328.0
      nominatorsArray[37] = 0.0; // 0.0/1
      nominatorsArray[38] = -201705688669067737041888000.0; // -10656.0/677.0
      nominatorsArray[39] = 134711371585528415289612000.0; // 78284.0/7447.0
      nominatorsArray[40] = 170040110221970637698424375.0; // 71865.0/5416.0
      nominatorsArray[41] = -164286600800369378703372000.0; // -2803372.0/218671.0
      nominatorsArray[42] = 13925492876493201861000000.0; // 963000.0/886193.0
      nominatorsArray[43] = 0.0; // 0.0/1
      nominatorsArray[44] = 0.0; // 0.0/1
      denominatorsArray[0] = 12814822750465358293671000.0;

      nominatorsArray[45] = 120898188125.0; // 28781.0/595840.0
      nominatorsArray[46] = 0.0; // 0.0/1
      nominatorsArray[47] = 0.0; // 0.0/1
      nominatorsArray[48] = 656706656256.0; // 820752.0/3128125.0
      nominatorsArray[49] = 100643412870.0; // 11259.0/280000.0
      nominatorsArray[50] = 753735186204.0; // 188245551.0/625100000.0
      nominatorsArray[51] = 503076015000.0; // 8667.0/43120.0
      nominatorsArray[52] = 261521308125.0; // 286011.0/2737280.0
      nominatorsArray[53] = 106319633420.0; // 5947.0/140000.0
      nominatorsArray[54] = 0.0; // 0.0/1
      denominatorsArray[1] = 2502900400000.0;
      // 10 stage formula of order (6,7)

      return 6;
    } 

  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY","SHARP6712_KEY") ) 
    {

      nominatorsArray.alloc(78);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 103527987476561421725952000.0; // 1.0/16.0
      nominatorsArray[1] = 22646747260497811002552000.0; // 7.0/512.0
      nominatorsArray[2] = 158527230823484677017864000.0; // 49.0/512.0
      nominatorsArray[3] = 67940241781493433007656000.0; // 21.0/512.0
      nominatorsArray[4] = 0.0; // 0.0/1
      nominatorsArray[5] = 203820725344480299022968000.0; // 63.0/512.0
      nominatorsArray[6] = 469045167750951747411456000.0; // 111.0/392.0
      nominatorsArray[7] = 0.0; // 0.0/1
      nominatorsArray[8] = -1673350328192584612386816000.0; // -99.0/98.0
      nominatorsArray[9] = 1825473085301001395331072000.0; // 54.0/49.0
      nominatorsArray[10] = 87231915373769346083904000.0; // 91.0/1728.0
      nominatorsArray[11] = 0.0; // 0.0/1
      nominatorsArray[12] = 0.0; // 0.0/1
      nominatorsArray[13] = 381732826251024830726144000.0; // 56.0/243.0
      nominatorsArray[14] = 255731170711135775271616000.0; // 2401.0/15552.0
      nominatorsArray[15] = 101899091578092856160256000.0; // 195409.0/3176523.0
      nominatorsArray[16] = 0.0; // 0.0/1
      nominatorsArray[17] = 0.0; // 0.0/1
      nominatorsArray[18] = 173057270080555139150643200.0; // 14934016.0/142943535.0
      nominatorsArray[19] = -84411855105725815635968000.0; // -29732.0/583443.0
      nominatorsArray[20] = 46090893393503927127244800.0; // 16368.0/588245.0
      nominatorsArray[21] = -355371146806073377873768875.0; // -442889.0/2064384.0
      nominatorsArray[22] = 0.0; // 0.0/1
      nominatorsArray[23] = 0.0; // 0.0/1
      nominatorsArray[24] = -3478540379212463769991987200.0; // -21.0/10.0
      nominatorsArray[25] = -1753978823350481333246949000.0; // -507449.0/479232.0
      nominatorsArray[26] = 2236430950281633149980135200.0; // 4790501.0/3548160.0
      nominatorsArray[27] = 4490267261329560970118041875.0; // 114320185.0/42172416.0
      nominatorsArray[28] = -200400575476091607024408000.0; // -10207975907029.0/84375901564416.0
      nominatorsArray[29] = 0.0; // 0.0/1
      nominatorsArray[30] = 0.0; // 0.0/1
      nominatorsArray[31] = -1927831887953949421181337600.0; // -8630826461408.0/7415850723435.0
      nominatorsArray[32] = -1201196455029963056486976000.0; // -18262259455163.0/25183623681216.0
      nominatorsArray[33] = 1639229303814386397768345600.0; // 47837870370299.0/48340360271280.0
      nominatorsArray[34] = 2755331129546902024951896000.0; // 19504514011591.0/11725708380672.0
      nominatorsArray[35] = 177204334817452722683904000.0; // 39976656.0/373688623.0
      nominatorsArray[36] = 0.0; // 124555217110727.0/164437126344288.0
      nominatorsArray[37] = 0.0; // 0.0/1
      nominatorsArray[38] = 0.0; // 0.0/1
      nominatorsArray[39] = 2315990435698177136079667200.0; // 11974507410944.0/8564433663765.0
      nominatorsArray[40] = 6295670119020227724979200000.0; // 11160304933175.0/2936377256148.0
      nominatorsArray[41] = -3610127545829472033268531200.0; // -55996991678797.0/25693300991295.0
      nominatorsArray[42] = -4851848169862687540939392000.0; // -3410248978351.0/1164277862496.0
      nominatorsArray[43] = -2328498633282804592017408000.0; // -114658850592.0/81566034893.0
      nominatorsArray[44] = 2462244180396565936250880000.0; // 424710.0/285719.0
      nominatorsArray[45] = 0.0; // -145638826059878.0/100078671191499.0
      nominatorsArray[46] = 0.0; // 0.0/1
      nominatorsArray[47] = 0.0; // 0.0/1
      nominatorsArray[48] = 0.0; // 708947812704256.0/346426169509035.0
      nominatorsArray[49] = 0.0; // -166431827237252.0/18381796749459.0
      nominatorsArray[50] = 9107910619295813043683328000.0; // 17245194808656.0/3136368612839.0
      nominatorsArray[51] = 4370932324326480571846656000.0; // 37326702095823.0/14145662519131.0
      nominatorsArray[52] = 7057758058836225406009344000.0; // 24792900992.0/5818865701.0
      nominatorsArray[53] = -5316366084149863678692556800.0; // -29736.0/9265.0
      nominatorsArray[54] = 454643268406841587743129600.0; // 235298.0/857285.0
      nominatorsArray[55] = 65879148252373328723558400.0; // 3583.0/90090.0
      nominatorsArray[56] = 0.0; // 0.0/1
      nominatorsArray[57] = 0.0; // 0.0/1
      nominatorsArray[58] = 0.0; // 0.0/1
      nominatorsArray[59] = 0.0; // 0.0/1
      nominatorsArray[60] = 552223243775150952560984064.0; // 2859008.0/8575875.0
      nominatorsArray[61] = 386222811945161854021632000.0; // 789929.0/3387879.0
      nominatorsArray[62] = -9986657374861812050165760.0; // -8192.0/1358775.0
      nominatorsArray[63] = 446223219015315613893525504.0; // 5152.0/19125.0
      nominatorsArray[64] = 171305093231812410591608832.0; // 1647086.0/15926625.0
      nominatorsArray[65] = 44580940780030399874088960.0; // 109.0/4050.0
      denominatorsArray[0] = 1656447799624982747615232000.0;

      nominatorsArray[66] = 9196038225.0; // 3583.0/90090.0
      nominatorsArray[67] = 0.0; // 0.0/1
      nominatorsArray[68] = 0.0; // 0.0/1
      nominatorsArray[69] = 0.0; // 0.0/1
      nominatorsArray[70] = 0.0; // 0.0/1
      nominatorsArray[71] = 77084573696.0; // 2859008.0/8575875.0
      nominatorsArray[72] = 53912654250.0; // 789929.0/3387879.0
      nominatorsArray[73] = -1394032640.0; // -8192.0/1358775.0
      nominatorsArray[74] = 62288081856.0; // 5152.0/19125.0
      nominatorsArray[75] = 23912394548.0; // 1647086.0/15926625.0
      nominatorsArray[76] = 6223031815.0; // 109.0/4050.0
      nominatorsArray[77] = 0.0; // 0.0/1
      denominatorsArray[1] = 231222741750.0;
      // 12 stage formula of order (6,7)

      return 6;
    } 

  if (integrationMethodDescription.checkForEnumValue 
      ("BUTCHER_ARRAY_NAME_KEY", "DOPRI7813_KEY") ) 
    {

      nominatorsArray.alloc(91);
      denominatorsArray.alloc(2);

      nominatorsArray[0] = 1.0/18.0;
      nominatorsArray[1] = 1.0/48.0;
      nominatorsArray[2] = 1.0/16.0;
      nominatorsArray[3] = 1.0/32.0;
      nominatorsArray[4] = 0.0;
      nominatorsArray[5] = 3.0/32.0;
      nominatorsArray[6] = 5.0/16.0;
      nominatorsArray[7] = 0.0;
      nominatorsArray[8] = -75.0/64.0;
      nominatorsArray[9] = 75.0/64.0;
      nominatorsArray[10] = 3.0/80.0;
      nominatorsArray[11] = 0.0;
      nominatorsArray[12] = 0.0;
      nominatorsArray[13] = 3.0/16.0;
      nominatorsArray[14] = 3.0/20.0;
      nominatorsArray[15] = 29443841.0/614563906.0;
      nominatorsArray[16] = 0.0;
      nominatorsArray[17] = 0.0;
      nominatorsArray[18] = 77736538.0/692538347.0;
      nominatorsArray[19] = -28693883.0/1125000000.0;
      nominatorsArray[20] = 23124283.0/1800000000.0;
      nominatorsArray[21] = 16016141.0/946692911.0;
      nominatorsArray[22] = 0.0;
      nominatorsArray[23] = 0.0;
      nominatorsArray[24] = 61564180.0/158732637.0;
      nominatorsArray[25] = 22789713.0/633445777.0;
      nominatorsArray[26] = 545815736.0/2771057229.0;
      nominatorsArray[27] = -180193667.0/1043307555.0;
      nominatorsArray[28] = 39632708.0/573591083.0;
      nominatorsArray[29] = 0.0;
      nominatorsArray[30] = 0.0;
      nominatorsArray[31] = -433636366.0/683701615.0;
      nominatorsArray[32] = -421739975.0/2616292301.0;
      nominatorsArray[33] = 100302831.0/723423059.0;
      nominatorsArray[34] = 790204164.0/839813087.0;
      nominatorsArray[35] = 800635310.0/3783071287.0;
      nominatorsArray[36] = 246121993.0/1340847787.0;
      nominatorsArray[37] = 0.0;
      nominatorsArray[38] = 0.0;
      nominatorsArray[39] = -37695042795.0/15268766246.0;
      nominatorsArray[40] = -309121744.0/1061227803.0;
      nominatorsArray[41] = -12992083.0/490766935.0;
      nominatorsArray[42] = 6005943493.0/2108947869.0;
      nominatorsArray[43] = 393006217.0/1396673457.0;
      nominatorsArray[44] = 123872331.0/1001029789.0;
      nominatorsArray[45] = -1028468189.0/846180014.0;
      nominatorsArray[46] = 0.0;
      nominatorsArray[47] = 0.0;
      nominatorsArray[48] = 8478235783.0/508512852.0;
      nominatorsArray[49] = 1311729495.0/1432422823.0;
      nominatorsArray[50] = -10304129995.0/1701304382.0;
      nominatorsArray[51] = -48777925059.0/3047939560.0;
      nominatorsArray[52] = 15336726248.0/1032824649.0;
      nominatorsArray[53] = -45442868181.0/3398467696.0;
      nominatorsArray[54] = 3065993473.0/597172653.0;
      nominatorsArray[55] = 185892177.0/718116043.0;
      nominatorsArray[56] = 0.0;
      nominatorsArray[57] = 0.0;
      nominatorsArray[58] = -3185094517.0/667107341.0;
      nominatorsArray[59] = -477755414.0/1098053517.0;
      nominatorsArray[60] = -703635378.0/230739211.0;
      nominatorsArray[61] = 5731566787.0/1027545527.0;
      nominatorsArray[62] = 5232866602.0/850066563.0;
      nominatorsArray[63] = -4093664535.0/808688257.0;
      nominatorsArray[64] = 3962137247.0/1805957418.0;
      nominatorsArray[65] = 65686358.0/487910083.0;
      nominatorsArray[66] = 403863854.0/491063109.0;
      nominatorsArray[67] = 0.0;
      nominatorsArray[68] = 0.0;
      nominatorsArray[69] = -5068492393.0/434740067.0;
      nominatorsArray[70] = -411421997.0/543043805.0;
      nominatorsArray[71] = 652783627.0/914296604.0;
      nominatorsArray[72] = 11173962825.0/925320556.0;
      nominatorsArray[73] = -13158990841.0/6184727034.0;
      nominatorsArray[74] = 3936647629.0/1978049680.0;
      nominatorsArray[75] = -160528059.0/685178525.0;
      nominatorsArray[76] = 248638103.0/1413531060.0;
      nominatorsArray[77] = 0.0;
      denominatorsArray[0] = 1.0;//the nominators will be too big for the double floating point number, so here we can't convert.

      nominatorsArray[78] = 14005451.0/335480064.0;
      nominatorsArray[79] = 0.0;
      nominatorsArray[80] = 0.0;
      nominatorsArray[81] = 0.0;
      nominatorsArray[82] = 0.0;
      nominatorsArray[83] = -59238493.0/1068277825.0;
      nominatorsArray[84] = 181606767.0/758867731.0;
      nominatorsArray[85] = 561292985.0/797845732.0;
      nominatorsArray[86] = -1041891430.0/1371343529.0;
      nominatorsArray[87] = 760417239.0/1151165299.0;
      nominatorsArray[88] = 118820643.0/751138087.0;
      nominatorsArray[89] = -528747749.0/2220607170.0;
      nominatorsArray[90] = 1.0/4.0;
      denominatorsArray[1] = 1.0;//the nominators will be too big for the double floating point number, so here we can't convert.
      // 13 stage formula of order (7,8)

	return 7;
 
    } 

  cerr << "Unknown name '" 
       << integrationMethodDescription.getEnum ("BUTCHER_ARRAY_NAME_KEY")
       << "' of the Butcher array. "
       << "If you really want to use it, please implement it at: "
       << WhereAmI
       << endl << Error::Exit;
    
  return 0;

}
