#include <iostream>
#include <mysql/mysql.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <ctime>

//using namespace std;

int main()
{
    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "*******", "********", "ovs", 3306, NULL, 0);
    if(conn){
        std::cout << "Connected \n";
    }
    else{
        std::cout <<"Not \n";
    }

    //MYSQL optData;
    std::string sqlQuery;
    int volType = -1;

    std::vector<int> eqIds; //List of eqIds-simple one level vector
    std::vector<int> bidCall;
    std::vector<int> askCall;
    std::vector<int> bidPut;
    std::vector<int> askPut;
    MYSQL_RES* qRes;
    MYSQL_ROW qRow;
    int Ccount = 0;
    int Pcount = 0;


    //----------------GETS CURRENT DATE---------------
    const int MAXLEN = 80;
    char cur_date[MAXLEN];
    time_t t = time(0);
    strftime(cur_date, MAXLEN, "%Y-%m-%d", localtime(&t));
    std::string cd = cur_date;
    //std::cout << cd << '\n';

    //----------HOLDER FOR EXP DATE -----------------
    std::string exp_date = "4/30/2020";
    //----------HOLDER FOR ANY TICKER ---------------
    std::string ticker_test = "UPS";
    


    if(volType < 0) { //Implied
      sqlQuery = "select date_ from ovscalendar where date_<='"+cd+"' and calType='2'";
      // std::cout << sqlQuery;
      mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
      qRes = mysql_store_result(conn);
      while(qRow=mysql_fetch_row(qRes)) {
         cd = qRow[0];
      }
        std::cout << cd;
        
   }

//    for(int i =0; i < askPut.size(); i ++){
//         std::cout << askPut[i] << " ";
//     }

    return 0;
    }