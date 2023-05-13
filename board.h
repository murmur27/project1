#include <fstream>

#include "page.h"

using std::endl;
using std::ofstream;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, char content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);
        void first_delete_process(int id);
        void second_delete_process(int id);

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
        }
    }

}

Board::~Board() {
    delete board;
    
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {
        
        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}


void Board::insert_page(int x, int y, int i_width, int i_height, int id, char content) {
    Page page(x,y,i_width,i_height,id,content);
    push_on_page(page,pages);//on_page
    pages.push_back(page);
    //below_page contents 저장.
    for (int h = y; h < (i_height+y); h++) {
        for (int w = x; w < (i_width+x); w++) {
            page.below_contents = new char[width*height];
            page.below_contents[h*width + w]=board[h*width + w];
        }
    }
    for (int h = y; h < (i_height+y); h++) {
        for (int w = x; w < (i_width+x); w++) {
            //insert_page할때 해당 자리에 다른 문자 있으면, on_page임을 표시.
            board[h*width + w] = content;
            //board의 각 content 자리에 해당 값이 바뀔 때마다, id리스트를 갱신하고, 각 자리마다 id log가 남도록 구성.
        }
    }
    Board::print_board();
}

"""void Board::delete_page(int id) {//pages에서, on_page에서 id 삭제해야함. 노노. process를 크게 세가지로 나누고 부분에 대해 recursive 먹이면된다.
    //recursive delete
    //on_page
    first_delete_process(id);//1st. delete process
    second_delete_process(id);//2nd. rebuilding process
    for (int i = 0; i < pages.size(); i++) {//3rd. delete id on vectors
            if (pages[i].get_id() == id) {
                pages.erase(pages.begin() + i - 1);
            }
        }
    for (int i = 0; i < pages.size();i++) {
    for (int j = 0; j < pages[i].on_pages.size(); j++) {
        auto it = pages[i].on_pages.begin(); // iterator 선언
        while (it != pages[i].on_pages.end()) {
            if (*it == id) {
                it = pages[i].on_pages.erase(it); // 해당 원소 삭제하고 iterator를 다음 원소로 이동
                } 
            else {
                ++it;
                }
            }
        }
    }
}"""

void Board::modify_content(int id, char content) {
    print_board();

}
void Board::modify_position(int id, int x, int y) {
    print_board();
    
}

void Board::first_delete_process(int id){
    Page del_page=Page::find_by_id(id,pages);
    int x=del_page.get_x();
    int y=del_page.get_y();
    int d_width=del_page.get_width();
    int d_height=del_page.get_height();
    if(del_page.on_pages.size()==0){
        for (int h = y; h < (d_height+y); h++) {
            for (int w = x; w < (d_width+x); w++) {
                board[h*width + w] = del_page.below_contents[h*width + w];//below_contents는 insert_page에서 declared 된다. 이후 주요함수들의 과정에서 편집됨.
        }
    }
        Board::print_board();
    }
    else{
        for(int i=0;i<del_page.on_pages.size();i++){
            Board::first_delete_process(del_page.on_pages[del_page.on_pages.size()-i-1]);
     }
    }
    for (int h = y; h < (d_height+y); h++) {
        for (int w = x; w < (d_width+x); w++) {
            board[h*width + w] = del_page.below_contents[h*width + w];
        }
    }
    Board::print_board();
}

void Board::second_delete_process(int id){
    Page del_page = Page::find_by_id(id,pages);
    if(del_page.on_pages.size()==0){
    }
    else{
        for(int i=0;i<del_page.on_pages.size();i++){//유사 insert_page 그렇지만, 새로운 page 생성파트와 push_back 부분은 빠짐.
            Page page=Page::find_by_id(del_page.on_pages[i],pages);
            int i_x;
            int i_y;
            int i_width;
            int i_height;
            int i_id;
            char i_content;
            for (int h = i_y; h < (i_height+i_y); h++) {
                for (int w = i_x; w < (i_width+i_x); w++) {
                    page.below_contents[h*width + w]=board[h*width + w];//below_contents update!
                }
            }
            for (int h = i_y; h < (i_height+i_y); h++) {
                for (int w = i_x; w < (i_width+i_x); w++) {
                    board[h*width + w] = i_content;//다시 채우기.
                }
            }
            Board::print_board();
            Board::second_delete_process(del_page.on_pages[i]);
        }
    }
}