
Compile OpenPHIGS with the following commands:

	make depend
	make

After these you will find the compilation results under distrib :

distrib/
	include/
		phigs/
	lib/
		X11/
			PEX/
	doc/
		PEX/
		OpenGL/
			glandx/
				intro/
					icons/
		PVT/
		PHIGS/
			man3/
			man7/
			draft/

For the installation, as root, execute the following commands:

	tar -C distrib -cvf - * | tar -C $OPEN_PHIGS_INSTALLATION_DIR -xvf -

where $OPEN_PHIGS_INSTALLATION_DIR is the basedir on which install OpenPHIGS
( i.e. /usr or /usr/local or /home/tux/myownPHIGS )

under tests/bin will be the tests programs
