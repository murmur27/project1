class Page {
    public:
        Page(int in_x,int in_y,int in_width,int in_height,int in_id,char in_content):x(in_x),y(in_y),width(in_width),height(in_height),id(in_id),content(in_content){

        }
        static int Page::find_page(int in_id){
            
        }
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content; 
};
