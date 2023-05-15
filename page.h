#include <vector>
#include <algorithm>
#include <stdio.h>
using namespace std;
class Page {
    public:
        Page(int in_x,int in_y,int in_width,int in_height,int in_id,char in_content,int board_width, int board_height):x(in_x),y(in_y),width(in_width),height(in_height),id(in_id),content(in_content),below_contents(new char[board_width*board_height]){
        }
        int get_x(){
            return x;
        }
        int get_y(){
            return y;
        }
        int get_width(){
            return width;
        }
        int get_height(){
            return height;
        }
        int get_id(){
            return id;
        }
        char get_content(){
            return content;
        }
        char* below_contents;
        static int find_by_id(int id, vector <Page> &pages) {//Page::find_by_id(id_to_find,pages)로 사용, order를 출력.
            for (int i = 0; i < pages.size(); i++) {
                if (pages[i].get_id() == id) {
                    return i;
                }
            }
        return -1;//error!!! 유의.
        }
        void modify_content(char modified_content){
            content=modified_content;
        }
        void modify_position_xy(int modified_x, int modified_y){
            x=modified_x;
            y=modified_y;
        }
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content;
};
//여기서 에러가 있다...
vector <int> on_page_ids(Page &current_page,vector <Page> &pages){//current_page를 넣으면 해당 page의 pages 위치에서 이후 page들만 비교하고, 먼저 겹치고, 안 겹치고를 이용해서 log 구하기.
    int x1=current_page.get_x();//위상 동일.
    int y1=current_page.get_y();
    int d_width1=current_page.get_width();
    int d_height1=current_page.get_height();
    int id1=current_page.get_id();
    vector <int> on_page_ids={};
    if(pages.size()==0){//예외 처리. 어차피 없긴해.
        return on_page_ids;
    }
    else{
        int current_page_order = Page::find_by_id(current_page.get_id(),pages);
        if(current_page_order==pages.size()-1){//예외 처리. 구하고자하는 on_pages의 id가 무조건 없음. 현재 페이지가 가장 마지막에 insert된 페이지일때.
            return on_page_ids;//size == 0
        }
        else{
        //만약에 pages에 이미 current_page가 섞여 있는 경우에 예외처리. current_page가 pages 상에 있는 index 이전까지에 대해 push_on_page를 수행하면된다.
            for (int i = current_page_order + 1; i < pages.size(); i++) {//pages 안의 원소들 중 current_page 이후 것들만 고려.
                int x2=pages[i].get_x();
                int y2=pages[i].get_y();
                int d_width2=pages[i].get_width();
                int d_height2=pages[i].get_height();
                int id2=pages[i].get_id();
                if((x2+d_width2-1<x1||x1+d_width1-1<x2)||(y2+d_height2-1<y1||y1+d_height1-1<y2)){//현재 페이지와 겹치지 않을 때의 조건.

                }
                else{   //겹칠 때.
                    int cross = 0;//겹치면 true == 1
                    if(on_page_ids.size()==0){//겹치는 처음 페이지는 무조건 추가.
                        cross = 0;//default
                    }
                    else{
                        for(int j = 0; j < on_page_ids.size(); j++){//on_page_ids에 들어있는 id에 해당하는 page와는 겹치면 안됨.
                            int x3=pages[Page::find_by_id(on_page_ids[j],pages)].get_x();//어딘가에 에러.
                            int y3=pages[Page::find_by_id(on_page_ids[j],pages)].get_y();
                            int d_width3=pages[Page::find_by_id(on_page_ids[j],pages)].get_width();
                            int d_height3=pages[Page::find_by_id(on_page_ids[j],pages)].get_height();
                            int id3=pages[Page::find_by_id(on_page_ids[j],pages)].get_id();
                            if((x2+d_width2-1<x3||x3+d_width3-1<x2)||(y2+d_height2-1<y3||y3+d_height3-1<y2)){//겹치지 않을 때의 조건.
                            }
                            else{//겹칠 때.
                                cross = 1;
                            }
                        }
                    }
                    if(cross == 0){//비교하려는 페이지가 on_pages_id에 저장된 것과 겹치지 않는다면,
                        on_page_ids.push_back(pages[i].get_id());
                    }
                }
            }
            sort(on_page_ids.begin(),on_page_ids.end());//id 오름차순으로 정렬해서 반환.
            return on_page_ids;
        }
    }
}

vector <Page> pages={};//initialization, size==0