BEGIN {
	split("1|\\a\\|3", x, "|"); print x[1]
	split("1|\\a\\|3", x, "\\|"); print x[1]
	split("1|\\a\\|3", x, "\\a"); print x[1]
	split("1|\\a\\|3", x, "\\a"); print x[1]
	split("1|\\a\\|3", x, "\\"); print x[1]
}
