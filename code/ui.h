#define MAX_LOG_MESSAGE_COUNT 1024
#define MAX_MARK_SIZE 64

typedef enum
{
    CameFrom_None,
        
    CameFrom_Inventory,
    CameFrom_Examine,
    CameFrom_Pickup
} CameFrom;

typedef struct
{
    u32 count;
    u32 start;
    u32 end;
} View;

typedef struct
{
    b32 render;
    u32 index;
    u32 duration_start;
    
    View view;
    char array[MAX_MARK_SIZE];
} Mark;

typedef struct
{
    Font *font;
    u32 font_newline;
    u32 window_offset;
    
    String128 log_messages[MAX_LOG_MESSAGE_COUNT];
    View short_log_view;
    
    b32 is_full_log_open;
    b32 is_full_log_view_set_at_end;
    View full_log_view;
    
    Mark mark;
    
    v4u defer_rect;
    Defer defer[MAX_DEFER_COUNT];
} UI;

internal void set_view_at_end(View *view);
internal void set_view_at_start(View *view);
internal void log_add(UI *ui, char *text, ...);
internal void update_view_scrolling(View *view, Input *input);
internal u32 get_view_range(View view);
internal b32 is_view_scrolling(View view, u32 count);