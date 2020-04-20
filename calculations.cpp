#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>


//TO DO:
// 3. Clean up Code.
// 4. Fix debugger info.

//Why is this happening to us!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ----------------Testing database connection and query---------------------

// ok make sure you include the header
//#include <mysql/mysql.h>
#include <mysql/mysql.h>

// using namespace MySql::Data::MySqlClient;

//Definitions for accessing MySQL database for reads
#define SQLADDRESS "localhost"
#define SQLUSERNAME "********"
#define SQLPASSWORD "*********"
#define SQLDB "ovs"
#define SQLPORT 3306
void database_test(float &volatility, std::string &issuer, std::string &currDate, float &currPrice, float &traDays, std::string ticker, std::string tarDate){


MYSQL* conn;
conn = mysql_init(0);
conn = mysql_real_connect(conn, SQLADDRESS, SQLUSERNAME, SQLPASSWORD, SQLDB, SQLPORT, NULL, 0);

//MYSQL optData;
std::string sqlQuery;
int volType = -1;

const int MAXLEN = 80;
char cDate[MAXLEN];
time_t t = time(0);
strftime(cDate, MAXLEN, "%Y-%m-%d", localtime(&t));
currDate = cDate;
   //std::vector<int> eqIds; //List of eqIds-simple one level vector
   MYSQL_RES* qRes;
   MYSQL_ROW qRow;
   if(volType < 0) { //Implied
    //Other queries dont't work if the markets haven't closed on the current trading day.
     sqlQuery = "select date_ from ovscalendar where date_<'"+currDate+"' and calType='2'";
     mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
      qRes = mysql_store_result(conn);
      while(qRow=mysql_fetch_row(qRes)) {
         currDate = qRow[0];
      }
      sqlQuery = "SELECT eqp.eqid,eqm.ticker,eqm.issuer,eqp.date_,eqp.close_, iv.ivMid FROM eqprice AS eqp LEFT JOIN eqmaster AS eqm ON eqm.eqid=eqp.eqid AND eqp.date_ between eqm.startDate AND eqm.endDate LEFT JOIN ivcmpr as iv on eqp.eqId = iv.eqId and eqp.date_=iv.date_ and iv.strike=100 WHERE eqm.ticker = '"+ticker+"' and eqp.date_='"+currDate+"'    LIMIT 1";
      mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
      qRes = mysql_store_result(conn);
      while(qRow=mysql_fetch_row(qRes)) {
        // eqIds.push_back(atoi(qRow[0]));
         volatility = atof(qRow[5]);
         issuer = qRow[2];
         currPrice = atof(qRow[4]);
         //std::cout << qRow[2] << "    "<< issuer << std::endl;

      }

      sqlQuery = "SELECT count(date_) AS days FROM ovscalendar WHERE date_ between \""+currDate+"\" AND \""+tarDate+"\" AND calType = 2";      
      mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
      qRes = mysql_store_result(conn);
      while(qRow=mysql_fetch_row(qRes)) {
          traDays = atof(qRow[0]);
          //std::cout << qRow[0] << " " << traDays << "\n";
      }
      
   }
}

//------------ End test function --------------------------------------

//Normal Distribution. Returns probability below target price.
/*
//May consider using call by reference if there is an issue with run-time later down the road
*/
void normDis(std::string ticker, float currPrice, float tarPrice, float vol, std::string currDate, std::string tarDate, float days, float periodsIaY, bool debug){
    float numerator, denominator;

    numerator = tarPrice/currPrice;
    numerator = log(numerator);
 
    
    denominator = vol*sqrt(days/252.0);//hard coded value change later
    float targetSD = numerator/denominator;
    float normalCDF;
    normalCDF = 0.5 * erfc(-targetSD * M_SQRT1_2);

	if(debug){
		std::cout << "Lognormal Probability Calculation Debugger Steps" << std::endl << std::endl;
		std::cout << "Ticker: " << ticker << 
				"\nCurrent Price: " << currPrice << 
				"\nVolatility: " << vol << 
				"\nCurrent Date: " << currDate <<
				"\nTarget Date: " << tarDate << 
				"\nTrading Days: " << days << 
				"\nPeriods in a year: " << periodsIaY <<
				"\nTARGET PRICE: " << tarPrice << 
				"\n\nNumerator: " << numerator << 
				"\nDenominator: " << denominator << 
				"\n\nTarget as a Standard Deviation: " << targetSD << 
				"\n\nProbability Below Target: " << normalCDF << 
				"\nProbability Above Target: " << 1.0-normalCDF << std::endl;

	}


    //Output Probability analysis to LogNormalAnalysis.txt
    //std::ofstream Analysis;

    //Analysis.open("LogNormalCalculations/LogNormalAnalysis.txt");

    std::cout << "PROBABILITY ANALYSIS" << std::endl;
	std::cout << "Ticker," << ticker << 
				"\nCurrent Price," << currPrice <<
                "\nTarget Price," << tarPrice << 
				"\nVolatility," << vol << 
				"\nCurrent Date," << currDate << 
				"\nTarget Date," << tarDate << 
				"\nProbability Below Target," << normalCDF << 
				"\nProbability Above Target," << 1.0-normalCDF << std::endl;

    //Analysis.close();
}

//Option Prices Probability
//Returns probability below target
void optionPrices(std::string ticker, float currPrice, float tarPrice, float vol, std::string currDate, std::string tarDate, float days, float periodsIaY, bool debug){
    float numerator, denominator;
    float x,x1,x2,y,y1,y2; 


}//optionPrices


void logNormGraph(float vol, float currPrice, float days, float periodsIaY){
    int length = 161;

    //Standard Deviation Array
    double stanDevs[length] = {};
    double sd = -4;
    for(int i = 0; i < length; i++){
        stanDevs[i] = sd;
        sd += 0.05;
    }

    //X-Axis values for graphing
    float xAxis[length] = {};
    /*
    //rootDYxV is a helper variable for xAxis calculations:
    //we use it for every xAxis calculation so we don't have to recalculate this value 160+ times
    //rootDYxV is the (square root of (trading days divided by periods in a year)) times volatility
    */
    float rootDYxV = sqrt(days/periodsIaY) * vol;
    for(int i = 0; i < length; i++){
        xAxis[i] = (exp (rootDYxV*stanDevs[i])) * currPrice;
        //cout << xAxis[i] << endl;
    }


    //Line values for graphing
    float yAxis[length] = {};

    float inverseRoot2Pi = 1/sqrt(2*M_PI);
    for(int i = 0; i < length; i++){
        yAxis[i] = inverseRoot2Pi * (exp (-1 * (pow(stanDevs[i], 2) /2)));

        /*
        //DO NOT DELTETE THIS COMMENT!!! MAY NEED THIS LATER
        //yAxis[i] = (float)(((int)((yAxis[i]+.00005)*10000))%100000)/10000;
        */
    }

    //Output all necessary data to a text file

    //std::ofstream graphData;

    //graphData.open("LogNormalCalculations/LogNormalGraphData.txt");

    //graphData << "X axis,Line" << endl;
    for(int i = 0; i < length; i++){  
        std::cout << xAxis[i] << "," << yAxis[i] << std::endl;
    }
    std::cout << "$PL1T" << std::endl;

    //graphData.close();
}



int main(int argc, char** argv){
    float periodsInaYear = 252;//This needs to be a float for later calculations

    /*
    //Command-Line Arguments:
    //argv[0] --> Executable Name (not important)
    //argv[1] --> Back Door Debugger Tool (0, or 1)
    //argv[2] --> Ticker Symbol (e.g. "JPM")
            //argv[3] --> Current Price (decimal format e.g. "133.37")
    //argv[3] --> Target Price (decimal format e.g. "135")
            //argv[5] --> Volatility (decimal format e.g. "0.2115")
            //argv[6] --> Current Date (date format e.g. "2/3/2020")
    //argv[4] --> Target Date (date format e.g. "3/20/2020")
            //argv[5] --> Trading Days (integer format e.g. "33")
    //
    //Example of how to compile via command-line
    //  g++ -std=c++11 -o ./public/calculations/test calculations.cpp -lmysqlclient -L/usr/include/mysql
    //
    //Example of how to run program via command-line:
    //  ./c 1 JPM 135 2020-04-24
    */

    if(argc < 5){
        std::cout << "Error: Invalid Arguments" << std::endl;
        return 0;
    }

    
    bool debugger;
    float volatility,currPrice,tarPrice;
    std::string ticker, currDate, tarDate;
    float traDays;//This needs to be a float for later calculations

    std::string::size_type sz;

    //ONLY NEED: debugger, ticker, target_date, target_price. (Unsure about trading_days)

    debugger = std::atoi(argv[1]);
    ticker = argv[2];
            //Can remove currprice, tarprice, volatility.
            //currPrice = std::atof(argv[3]);
    tarPrice = std::atof(argv[3]);
            //volatility = std::atof(argv[5]);
            //currDate = argv[5];
    tarDate = argv[4];
            //traDays = std::atof(argv[5]);
    
    
    std::string issuer = "";
    //float v = 0.0;


    //NEEDS TO RETURN: volatility, currentprice, issuer.
    database_test(volatility, issuer, currDate, currPrice, traDays, ticker,tarDate);
    //std::cout << issuer << "    " << volatility << std::endl;
    logNormGraph(volatility, currPrice, traDays, periodsInaYear);
    normDis(ticker, currPrice, tarPrice, volatility, currDate, tarDate, traDays, periodsInaYear, debugger);


    
    // for(int i = 0; i < argc; i++){
    //     std::cout << argv[i] << std::endl;
    // }
    // std::cout << issuer << std::endl;
    // std::cout << volatility << std::endl;
    // std::cout << currDate << std::endl;
    // std::cout << currPrice << std::endl;



    return 0;

}
