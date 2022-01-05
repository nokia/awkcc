func g() { return "{" f() "}" }
func f() { return $1 }
 { print "<" g() ">" }
