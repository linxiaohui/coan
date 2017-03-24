/**ARGS: source -DFOO */
/**SYSCODE: = 2 */

#define OTHER_CLIENT_HAS_LOCK() \
(\
( isr_blk == &_buffer ) \
  ? (_buffer_1._status == _GRANTED) || (_buffer_1._status == _RETAINED_FOR_ACCESS) \
  : (_buffer._status == _GRANTED) || (_buffer._status == _RETAINED_FOR_ACCESS) \
)

#define CHECK_NEITHER_CLIENT_HAS_LOCK() \
( (_buffer._status != _GRANTED) && \
  (_buffer._status != _RETAINED_FOR_ACCESS) && \
  (_buffer_1._status != _GRANTED) && \
  (_buffer_1._status != _RETAINED_FOR_ACCESS) )

#define CHECK_NEITHER_CLIENT_IS_ABORTING_LOCK() \
( (_buffer._status != _ABORTING) && (_buffer_1._status != _ABORTING) )

