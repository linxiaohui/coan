/**ARGS: source */
/**SYSCODE: = 2 */
/* Problem code from Tony Whitely */

#define MSG_DUP_PARM MSG_HIGH("Duplicate parm: %d", parm_id, 0,0)

#define SIZE_CHECK  \
         { if( pos > rec_len ) \
           { \
             MSG_ERROR("Encode SMSP err: pos=%d>rec_len=%d", pos, rec_len, 0); \
             return FALSE; \
           } \
         }

