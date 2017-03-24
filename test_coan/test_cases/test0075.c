/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 0 */
// None of these comments should cause an unbalanced quote error
// This comment has contains an unclosed "-character.
// This comment doesn't but it contains an unclosed single quote.
/* The following comment terminator isn't quoted by the previous apostrophe. */
