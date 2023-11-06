void mainLoop(const char*);

int main(int argc, const char* argv[]) {
	mainLoop(argc >= 2 ? argv[1] : 0);
}
