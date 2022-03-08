/*
 * @Author       : cyk
 * @Date         : 2022-03-02
 * @Email        : 2371208450@qq.com
 * @TODO         : support dont care 
 */
 #include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <algorithm>
#include "fort.hpp"

#define TABLE

using namespace std;
    /*
        工具类
    */
class Util{

public:
    string intToBinString(int size, int val){
        string bin;
        bin = bitset<100>(val).to_string();
        bin = bin.substr(100-size);
        return bin;
    }
    /*
        返回二进制串中1的数量
    */
    int get1s(string x) {
        // returns the number of 1s in a binary string
        int count = 0;
        for (int i = 0; i < x.size(); ++i){
            if (x[i] == '1')
                count++;
        }
        return count;
    }
    /*
        检查两个二进制串是否只有一位不同
    */
    bool compare(string a, string b) {
        
        int count = 0;
        for(int i = 0; i < a.length(); i++) {
            if (a[i]!=b[i])
                count++;
        }

        if(count == 1)
            return true;

        return false;
    }
    /*
        把只有一位不同的位置用‘-’替代
    */
    string getDiff(string a, string b) {
        
        for(int i = 0; i < a.length(); i++) {
            if (a[i]!=b[i])
                a[i]='-';
        }

        return a;
    }
    /*
        判空
    */    
    bool checkEmpty(vector< vector< string> > table){
        for (int i = 0; i < table.size(); ++i){
            if (table[i].size()!=0) {
                return false;
            }
        }
        return true;
    }

    string binToString(string x){
        // converts binary string to alphabetic variables
        string out = "";
        for (int i = 0; i < x.size(); ++i){
            if (x[i]=='1') {
                char x = 65 + i;
                out += x;
            }
            else if (x[i]=='0') {
                char x = 65 + i;
                out += x;
                out += "'";
            }
        }
        return out;
    }
    int binToInt(string bin){
        int base = 1, a = 0;
        for(int i = bin.size()-1; i >= 0; i--){
            a += base * (bin[i] - '0');
            base *= 2;
        }
        return a;
    }
    /*
        判断prime implicant属于哪个最小项（建prime implicant时用）
        Example：
        ╔══════╤═══╤═══╤═══╤═══╤═══╗
        ║      │ 2 │ 3 │ 4 │ 5 │ 6 ║
        ╠══════╪═══╪═══╪═══╪═══╪═══╣
        ║ 0-10 │ X │   │   │   │ X ║
        ╟──────┼───┼───┼───┼───┼───╢
        ║ 001- │ X │ X │   │   │   ║
        ╟──────┼───┼───┼───┼───┼───╢
        ║ 01-0 │   │   │ X │   │ X ║
        ╟──────┼───┼───┼───┼───┼───╢
        ║ 010- │   │   │ X │ X │   ║
        ╚══════╧═══╧═══╧═══╧═══╧═══╝
    */
    bool primeIncludes(string imp, string minTerm){
       
        for (int i = 0; i < imp.size(); ++i){
            if (imp[i]!='-'){
                if(imp[i]!=minTerm[i]){
                    return false;
                }
            }
        }
        return true;
    }
    /*
        计算逻辑函数中每个prime implicant变量的个数
    */
    int getVar(set<int> comb, vector<string> primeImp){

        int count = 0;
        set<int> :: iterator itr;
        for (itr = comb.begin(); itr != comb.end(); ++itr){
            int imp = *itr;
            for (int i = 0; i < primeImp[imp].size(); ++i){
                if (primeImp[imp][i]!='-')
                    count ++;
            }
        }
        return count;

    }

};

class QM{
private:
    Util util;
    vector< int > minInt_; //最小项的十进制形式
    vector< string > minBin_; //最小项的二进制形式
    int nBits_; 
    int nMin_;  
    vector< vector< string> > table;
    vector< string > primeImp;
    vector< set<int> > functions;

public:

    QM(int nBits, int nMin, vector<int> minInt)
    :nBits_(nBits),nMin_(nMin),minInt_(minInt){

        for(int i = 0; i < nMin_; i++){
            minBin_.push_back(util.intToBinString(nBits_, minInt_[i]));
        }
#ifdef DEBUG        
        for (int i = 0; i < nMin_; ++i){
            cout << i << ") " << minBin_[i] << endl;
        }
#endif
        table = vector< vector< string> >(nBits_ + 1);
    }
    ~QM(){

    }
    void printResult(){
        this->setPrimeImp();
        this->minimise();
        this->displayFunctions();
    }

    void setPrimeImp() {
        set< string > primeImpTemp;
        createTable();

#ifdef DEBUG
        cout << "\nGetting Prime Implicants.." << endl;
#endif
    /*
        Q-M算法核心，找出所有prime implicant
    */
        while (!util.checkEmpty(table)){
            table = combinePairs(table, primeImpTemp);
        }

        set<string > :: iterator itr;  
        for (itr = primeImpTemp.begin(); itr != primeImpTemp.end(); ++itr){
            string x = *itr;
            primeImp.push_back(x);
        }
#ifdef DEBUG
        cout << "\nThe Prime Implicants are:" << endl;
        for (int i = 0; i < primeImp.size(); ++i){
            cout  << i << ") "<< util.binToString(primeImp[i]) << endl;
        }
#endif
    }

    void minimise() {
        /*
            建prim implicant chart
        */
        bool primeImpChart[primeImp.size()][nMin_] = {{false}};

        for (int i = 0; i < primeImp.size(); ++i){
            for (int j = 0; j < nMin_; ++j){
                primeImpChart[i][j] = util.primeIncludes(primeImp[i], minBin_[j]);
            }
        }
#ifdef DEBUG
        cout << "\nPrime implicants chart:" << endl;
        for (int i = 0; i < primeImp.size(); ++i){
            for (int j = 0; j < nMin_; ++j){
                if (primeImpChart[i][j] == true){
                    cout << "1\t";
                }
                else {
                    cout << "0\t";
                }
            }
            cout << endl;
        }
#endif

#ifdef TABLE
    fort::char_table tablePrint;
    /* Change border style */
    tablePrint.set_border_style(FT_DOUBLE2_STYLE);
    tablePrint << fort::header;
    tablePrint[0][0] = " ";
    for(int i = 0; i < nMin_; i++){
        
        tablePrint[0][i+1] = to_string(util.binToInt(minBin_[i]));
    }
    for(int i = 0; i < primeImp.size(); i++){
        tablePrint[i+1][0] = primeImp[i];
    }
    for (int i = 0; i < primeImp.size(); ++i){
        for (int j = 0; j < nMin_; ++j){
            if (primeImpChart[i][j] == true){
                tablePrint[i+1][j+1] = "X";
            }
            else {
                tablePrint[i+1][j+1] = " ";
            }
        }
            cout << endl;
        }
    tablePrint << fort::endr;
    std::cout << tablePrint.to_string() << std::endl;
#endif
        // Petric Method
        vector< set<int> > patLogic;
        for (int j = 0; j < nMin_; ++j){ 
            set<int> x;
            for (int i = 0; i < primeImp.size(); ++i){ 
                if (primeImpChart[i][j] == true) {
                    x.insert(i);
                }
            }
            patLogic.push_back(x);
        }
#ifdef DEBUG
        cout << "\nPetric logic is : " << endl;
        for (int i = 0; i < patLogic.size(); ++i){
            set<int > :: iterator itr;  
            for (itr = patLogic[i].begin(); itr != patLogic[i].end(); ++itr){
                int x = *itr;
                cout << x << " ";
            }
            cout << endl;
        }
#endif
        /*
            找出所有覆盖（回溯）
        */
        set< set<int> > posComb;
        set<int> prod;
        getPosComb(patLogic, 0, prod, posComb); 
        int min = 9999;

        cout << "\nPossible combinations" << endl;
        set< set<int> > :: iterator itr1;
        for (itr1 = posComb.begin(); itr1 != posComb.end(); ++itr1){
            set<int> comb = *itr1;
            if (comb.size() < min){
                min = comb.size();
            }
            set<int > :: iterator itr;
            for (itr = comb.begin(); itr != comb.end(); ++itr){
                int x = *itr;
                cout << x << " ";
            }
            cout << endl;
        }
#ifdef DEBUG
        cout << "\nGetting the combinations with min terms and min variables ..." << endl;
#endif
        /*
            从得到的set中找最小覆盖
        */
        vector<set<int>> minComb;
        set< set<int> > :: iterator itr;
        for (itr = posComb.begin(); itr != posComb.end(); ++itr){
            set<int> comb = *itr;
            if (comb.size() == min) {
                minComb.push_back(comb);
            }
        }


        min = 9999;
        for (int i = 0; i < minComb.size(); ++i){
            if(util.getVar(minComb[i], primeImp) < min){
                min = util.getVar(minComb[i], primeImp);
            }
        }

        for (int i = 0; i < minComb.size(); ++i){
            if(util.getVar(minComb[i], primeImp) == min){
                functions.push_back(minComb[i]);
            }
        }
    }

    void displayFunctions() {
        cout << "\nPossible functions \n" << endl;
        for (int i = 0; i < functions.size(); ++i){
            set<int> function = functions[i];
            set<int> :: iterator itr;
            cout << "Function " << i+1 << ":"<< endl;
            for (itr = function.begin(); itr != function.end(); ++itr){
                int x = *itr;
                cout << util.binToString(primeImp[x]) << " + ";
            }
            cout << "\b\b  \n" << endl;
        }
    }
     /*
        回溯穷举所有可能的覆盖，最后从中找最小覆盖
    */
    void getPosComb(vector< set<int> > &patLogic, int level, set<int> prod, set< set<int> > &posComb) {

        if (level == patLogic.size()){
            set<int> x = prod;
            posComb.insert(x);
            return;
        }
        else{
            set<int > :: iterator itr;
            for (itr = patLogic[level].begin(); itr != patLogic[level].end(); ++itr){

                int x = *itr;
                bool inserted = prod.insert(x).second;
                getPosComb(patLogic, level+1, prod, posComb);
                if (inserted){
                    prod.erase(x);
                }
            }
        }
    }

    vector< vector< string> > combinePairs(vector< vector< string> > table, set<string>& primeImpTemp) {
        bool checked[table.size()][nMin_] = {false};
        vector< vector< string> > newTable(table.size()-1);


        for (int i = 0; i < table.size() -1; ++i){
            for (int j = 0; j < table[i].size(); ++j){
                for (int k = 0; k < table[i+1].size(); k++){
                    if (util.compare(table[i][j], table[i+1][k])){
                        newTable[i].push_back(util.getDiff(table[i][j], table[i+1][k]));
                        checked[i][j] = true;
                        checked[i+1][k] = true;
                    }
                }
            }
        }

        for (int i = 0; i < table.size(); ++i){
            for (int j = 0; j < table[i].size(); ++j){
                if (!checked[i][j]) {
                    primeImpTemp.insert(table[i][j]);
                }
            }
        }
        return newTable;
    }

    void createTable() {
        for (int i = 0; i < nMin_; ++i){
            int num1s = util.get1s(minBin_[i]);
            table[num1s].push_back(minBin_[i]);
        }
#ifdef TABLE        
    fort::char_table tablePrint;
    /* Change border style */
    tablePrint.set_border_style(FT_DOUBLE2_STYLE);
    tablePrint << fort::header << "Group" << "Min Terms"<<"Binary"<<fort::endr;
        for (int i = 0; i < nBits_ + 1; ++i){
            for (int j = 0; j < table[i].size(); ++j){
                tablePrint << i << util.binToInt(table[i][j])<<table[i][j]<<fort::endr;
            }
        }
    /* Set center alignment for the 1st and 3rd columns */
    tablePrint.column(1).set_cell_text_align(fort::text_align::center);
    tablePrint.column(3).set_cell_text_align(fort::text_align::center);

    std::cout << tablePrint.to_string() << std::endl;

#endif
    }

};

int main()  {
    int nbits;
    cout << "Enter number of bits" << endl;
    cin >> nbits;
    int nmin;
    cout << "Enter number of min terms:" << endl;
    cin >> nmin;

    cout << "Enter min terms:" << endl;
    vector<int> minInt;
    for (int i = 0; i < nmin; ++i){
        int x;
        cin >> x;
        minInt.push_back(x);
    }
    QM qm(nbits, nmin, minInt);
    qm.printResult();
    return 0;
}