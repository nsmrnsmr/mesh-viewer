#ifndef INPUT_2022_02_17
#define INPUT_2022_02_17

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>

char *strDup(const char *src){
  unsigned int sz = std::strlen(src) + 1;
  char *dst = new char[sz];
  char *dsth = dst;
  while((*dst++ = *src++) != '\0'){}
  return dsth;
}

void readPointIdx(char *s, int &v){
  v = -1; 
  int vt, vn;
       if(sscanf(s, "%d/%d/%d", &v, &vt, &vn) == 3){ --v;}
  else if(sscanf(s, "%d/%d"   , &v, &vt     ) == 2){ --v;}
  else if(sscanf(s, "%d//%d"  , &v, &vn     ) == 2){ --v;}
  else if(sscanf(s, "%d"      , &v          ) == 1){ --v;}
}

template<typename V, typename F>
void readOBJ(const std::string filename, std::vector<V> &vertices, std::vector<F> &faces){
  std::ifstream f(filename);
  if(!f.is_open()){
      std::cerr << "ERROR: " << __FILE__ << ", line " << __LINE__ << ": readOBJ() : can not open input file. :" << filename << "\n";
      exit(-1);
  }

  std::string line;
  while(std::getline(f, line)){
    switch(line[0]){
      case 'v':{
        float a,b,c;
             if(sscanf(line.data(), "v  %f %f %f", &a, &b, &c) == 3) vertices.emplace_back(a,b,c);
        break;
      }
      case 'f':{
        line = line.substr(1, line.size()-1);
        std::istringstream ss(line);
        F p_pos, p_uv, p_nor;
        int i = 0;
        for(std::string sub_str; ss >> sub_str;){
          int v_pos = -1;
          readPointIdx(strDup(sub_str.data()), v_pos);
          if(v_pos >= 0) p_pos.push_back(v_pos);
          i++;
        }
        if(!p_pos.empty()){
          faces.push_back(p_pos);
          //faceCol.push_back(Col); Color Qtならいけるがそれ以外はどうするか考える
        }
        break;
      }
      case 'u':{//material usemtl 処理用
        break;
      }
      case 'm':{//material mtllib 処理用
        break;
      }
    }
  }
  f.close();
}

template<typename V, typename F>
void readOFF(const std::string filename, std::vector<V> &vertices, std::vector<F> &faces)
{
  std::ifstream f(filename);
  if(!f.is_open()){
      std::cerr << "ERROR: " << __FILE__ << ", line " << __LINE__ << ": readOFF() : can not open input file. :" << filename << "\n";
      exit(-1);
  }

  std::string line;

  int v_num, f_num;
  for(int i=0; i<2; ++i){
    std::getline(f, line);
    if(i == 1) sscanf(line.data(), "%d %d 0", &v_num, &f_num);
  }

  for(int i=0; i<v_num; ++i){
    std::getline(f, line);
    double x, y, z;
    sscanf(line.data(), "%lf %lf %lf", &x, &y, &z);
    vertices.emplace_back(x,y,z);
  }

  for(int i=0; i<f_num; ++i){
    std::getline(f, line);
    int a[3];
    sscanf(line.data(), "3 %d %d %d", &a[0], &a[1], &a[2]);
    faces.emplace_back(a[0],a[1],a[2]);
  }
  f.close();
}
#endif
