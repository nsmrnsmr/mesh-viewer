#ifndef FACE_H
#define FACE_H

struct Face{
    Face(const int a=-1, const int b=-1, const int c=-1) :
        v0(a),
        v1(b),
        v2(c)
    {};

    void push_back(const int &a){
        if(v0 < 0) v0 = a;
        else if(v1 < 0) v1 = a;
        else v2 = a;
    }

    void push(const int &i, const int &a){
        switch (i) {
            case 0 :{
                v0 = a;
                break;
            }
            case 1 :{
                v1 = a;
                break;
            }
            case 2 :{
                v2 = a;
                break;
            }
        }
    }

    bool empty(){
        if(v0 == -1) return true;
        return false;
    }
    int v0, v1, v2;
};

#endif // FACE_H
