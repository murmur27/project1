#include <vector>
using namespace std;
class Page {
    public:
        Page(int in_x,int in_y,int in_width,int in_height,int in_id,char in_content):x(in_x),y(in_y),width(in_width),height(in_height),id(in_id),content(in_content){
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
        static Page find_by_id(int id, vector <Page> pages) {//Page::find_by_id(id_to_find,pages)로 사용
        for (int i = 0; i < pages.size(); i++) {
            if (pages[i].get_id() == id) {
                return pages[i];
            }
        }
        return pages[0];//error
        }
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content; 
};
vector <Page> pages;