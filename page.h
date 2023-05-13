#include <vector>
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
        vector <int> on_pages;//member variable
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
        void modify_position(int modified_x, int modified_y){
            x=modified_x;
            y=modified_y;
        }
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content;
};
void push_on_page(Page current_page,vector <Page> &pages){//current_page를 넣으면 해당 페이지의 below_page들 중 가장 위의 것을 골라서 해당 below_page의 on_pages vector에 on_page id 값 추가.
    int x1=current_page.get_x();//위상 동일.
    int y1=current_page.get_y();
    int d_width1=current_page.get_width();
    int d_height1=current_page.get_height();
    int id1=current_page.get_id();
    int updated_order=-1;
    int there_is_same_id=0;
    int limit_searching_order;
    if(pages.size()==0){
    }
    else{
        for (int i = 0; i < pages.size(); i++) {//identify whether there is same id
            int id2=pages[i].get_id();
            if(id1==id2){
                there_is_same_id=1;
                limit_searching_order=i;
            }
        }
        if(there_is_same_id==0){
            limit_searching_order=pages.size();
        }
        //만약에 pages에 이미 current_page가 섞여 있는 경우에 예외처리. current_page가 pages 상에 있는 index 이전까지에 대해 push_on_page를 수행하면된다.
        for (int i = 0; i < limit_searching_order; i++) {//pages 안의 원소들 중에서 가장 위에 있는 것(pages vector에서 가장 뒤에 것)을 선택.
            int x2=pages[i].get_x();
            int y2=pages[i].get_y();
            int d_width2=pages[i].get_width();
            int d_height2=pages[i].get_height();
            int id2=pages[i].get_id();
            if(((x2+d_width2-1<x1||x1+d_width1-1<x2)||(y2+d_height2-1<y1||y1+d_height1-1<y2))||id1==id2){//on_page가 아닐 때의 조건.

            }
            else{//on_page일때.
                    //일단은 current_page가 하나의 매칭만 가능하다고 생각한다. 주어진 case가 어떤 것을 반영하는 지는 실험적으로 확인.
                    //추후에 error가 발생한다면 이부분 유의.
                updated_order=i;
            }
        }
        if(updated_order>=0){
            pages[updated_order].on_pages.push_back(current_page.get_id());
        }
    }
}
//on_page가 1차 vector로 나타남.
vector <Page> pages={};//initialization, size==0