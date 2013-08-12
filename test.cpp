#include "hypermat.h"
#include <assert.h>
#include <iostream>

using std::cout;
using std::endl;

double mul(double* a){
    return (a[0])*(a[1]);
}

double multi(double a, double b){
    double ret[2] = {a,b};
    double ex = mul(ret);
    return ex;
    return 1;
}

template<class contentType>
hypermat<contentType> testconst1(){
    int in[3]={1,2,3};
    double in2[6]={1,2,3,4,5,6};
    hypermat<double> mat1(3,in);
    hypermat<contentType> mat2(3,in,6,in2);
    hypermat<contentType> mat3(mat1);
    int in3[3]={4,1,3};
    hypermat<contentType> mat4(3, in3);
    hypermat<contentType> matin[1] = {mat4};
    mat2.map(mat4, multi);
    assert(mat2.map(mat4, multi)==mat2.map(1,matin, mul));
    cout<<"bargh"<<endl;
    int bla=3;
    assert(mat1.num_dim_==bla);
    for(int i=0;i<3;i++) assert(mat1.dim_[i]==i+1);
    for(int i=0;i<6;i++) assert(mat2.interval_[i]==i+1);
    return mat2;
}
    
template <class contentType>
int testadd(hypermat<contentType> mata, hypermat<contentType> matb){
    hypermat<contentType> jeck=(mata+matb);
    hypermat<contentType> ret(mata);
    for(int i=0;i<mata.interval_.len();i++){
        ret.interval_[i]=mata.interval_[i]+matb.interval_[i];
        assert(ret.interval_[i]==jeck.interval_[i]);
    }
    assert(ret==(mata+matb));
    return 1;
}

template <class contentType>
int testsubst(hypermat<contentType> mata, hypermat<contentType> matb){
    hypermat<contentType> jeck=(mata-matb);
    hypermat<contentType> ret(mata);
    for(int i=0;i<mata.interval_.len();i++){
        ret.interval_[i]=mata.interval_[i]-matb.interval_[i];
        assert(ret.interval_[i]==jeck.interval_[i]);
    }
    assert(ret==jeck);
    return 1;
}

int main(){
    cout << "this" <<endl;
    hypermat<double> mat(testconst1<double>());
    cout<<"bargh"<<endl;
    int in[3]={1,2,3};
    double in2[6]={1,2,3,4,5,6};
    hypermat<double> mat2(3,in,6,in2);
    assert(mat2.num_dim_==3);
    assert(mat2.dim_[2]==3);
    +mat2;
    cout<<"bargh"<<endl;
    for(int i=0;i<6;i++)assert(mat2.interval_[i]==mat.interval_[i]);
    int ret1=testadd(mat, mat);
    int ret2=testsubst(mat, mat);
    int arg[2]={4,5};
    double arg2[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int arg3[2]={2,6};
    int arg4[12]={1,2,3,4,0,0,5,4,3,2,1,0};
    hypermat<int> argument(2,arg3,12,arg4);
    hypermat<double> selection(2,arg,20,arg2);
    hypermat<double> ri(selection.selection(argument));// = run;
    hypermat<double> pi(ri.selection(argument));
    //~ for(int i=0;i<20;i++)pi.interval_[i]=i;
    pi.interval_[0]=9999;
    for(int i=0;i<20;i++) cout<<selection.interval_[i]<<endl;
    cout << "hi" << endl;
    return 1;
}
