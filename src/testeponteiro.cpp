#include <string>
#include <vector>
#include <iostream>
using namespace std;
class teste
{
    int i ;
    string str;
    char c;
    public:
    teste(int _i,string _str, char _c)
    {
        i = _i;
        str = _str;
        c = _c;
    }
    int getint()
    {
        return i;
    }
    string getstr()
    {
        return str;
    }
    char getchar()
    {
        return c;
    }
    void setint(int _i)
    {
        i = _i;
    }
    void setstr(string _str)
    {
        str = _str;
    }
    void setchar(char _c)
    {
        c = _c;
    }

};
void printvec(vector<teste> *vetor)
{
    cout << vetor->at(0).getint() << endl;
    cout << vetor->at(0).getchar() << endl;
    cout << vetor->at(0).getstr() << endl;

}
void alteravec(vector<teste> *vetor,int inteiro,string str2,char letra)
{
    vetor->at(0).setint(inteiro);
    vetor->at(0).setchar(letra);
    vetor->at(0).setstr(str2);
    printvec(vetor);
}

void alteravec2(vector<teste> *vetor,int inteiro,string str2,char letra)
{
    alteravec(vetor,inteiro,str2,letra);
}
int main()
{
    vector<teste> vetor;
    int a = 1;
    string b = "bbbb";
    char d = 'd';

    vetor.push_back(teste(a,b,d));

    alteravec(&vetor,2,"aaaaa",'l');
    alteravec2(&vetor,6,"gggggg",'p');

}