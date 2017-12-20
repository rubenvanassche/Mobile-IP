all: build run

build: copy
	cd click; make elemlist; make -j2

configure:
	cd click; ./configure --disable-linuxmodule --enable-local --enable-etherswitch

copy:
	cp -R elements/. click/elements/local/mobile-ip

app:
	cd click/apps/ClickController; java ClickController localhost 1981

test: build
	cd click/scripts; sudo ../userlevel/click test.click -p 1981

debug:
	cd click/scripts;  gdb ../userlevel/click

setup:
	cd click/scripts; sudo ./setup.sh

run:
	cd click/scripts; sudo ./start_click.sh

reference: build
	cd click-reference/solution; sudo ./start_click.sh

th:
	cd click/scripts; ./to_home.sh

tf:
	cd click/scripts; ./to_foreign.sh

appMN:
	cd click/apps/ClickController; java ClickController localhost 10001

appHA:
	cd click/apps/ClickController; java ClickController localhost 10002

appFA:
	cd click/apps/ClickController; java ClickController localhost 10003

appTest:
	cd click/apps/ClickController; java ClickController localhost 1981

kill:
	sudo killall -9 click
