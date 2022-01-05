#awkcchint -b
{
	print $0 > "foo"
	system("echo HELLO;cat foo")
}
