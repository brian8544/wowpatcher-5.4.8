#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>

struct Patch {
	size_t offset;
	std::vector<uint8_t> original;
	std::vector<uint8_t> patched;
	std::string description;
};

std::vector<Patch> patches = {
	{0x8DFA, {0xE8, 0x41, 0xB3, 0xFF, 0xFF}, {0x90, 0x90, 0x90, 0x90, 0x90}, "NOP signature verification call."},

	{0x65C7FB, {0xC8}, {0x01}, "Remove patch number limit."},
	{0x65C808, {0x7D}, {0xEB}, "Unlock FrameXML and Blizzard interface addons."},
	{0x7D7E71, {0x06}, {0x38}, "Bypassing FrameXML modification detection."},

	{0x3A3C60, {0x40}, {0x00}, "CDN auth - clear authentication parameter."},
	{0x5AC968, {0x01}, {0xFF}, "CDN receive - raise acceptable size threshold."},
	{0x5AC96A, {0x85}, {0x84}, "CDN receive - invert size validation branch."},
	{0x5AD5DE, {0x44, 0x8B, 0x44, 0x24, 0x50}, {0x45, 0x31, 0xC0, 0x90, 0x90}, "CDN request - zero R8D argument before send."},
	{0xA83E39, {0xE8, 0x92, 0xF3, 0xFF, 0xFF}, {0x90, 0x90, 0x90, 0x90, 0x90}, "CDN - NOP content signature validation call."},
	{0xA8401D, {0x74}, {0xEB}, "CDN auth - force unconditional jump past auth check."},
};

void waitForEnter(const std::string& msg = "") {
	if (!msg.empty()) std::cout << msg << "\n";
	std::cout << "Press Enter to exit...";
	std::cin.get();
}

void printOffset(std::ostream& os, size_t offset) {
	os << "0x" << std::uppercase << std::hex
		<< std::setw(8) << std::setfill('0') << offset
		<< std::nouppercase << std::dec;
}

bool verify(const std::vector<uint8_t>& data, const Patch& p) {
	if (p.offset + p.original.size() > data.size()) {
		std::cout << "[FAIL] ";
		printOffset(std::cout, p.offset);
		std::cout << " - " << p.description << " (offset out of range)\n";
		return false;
	}
	if (!std::equal(p.original.begin(), p.original.end(), data.begin() + p.offset)) {
		std::cout << "[FAIL] ";
		printOffset(std::cout, p.offset);
		std::cout << " - " << p.description << " (bytes mismatch)\n";
		return false;
	}
	return true;
}

void apply(std::vector<uint8_t>& data, const Patch& p) {
	std::copy(p.patched.begin(), p.patched.end(), data.begin() + p.offset);
	std::cout << "[PATCHED]  ";
	printOffset(std::cout, p.offset);
	std::cout << " - " << p.description << "\n";
}

int main() {
	const std::string inputPath = "Wow-64.exe";
	const std::string outputPath = "Wow-64-patched.exe";

	std::cout << "WoW 5.4.8.18414 x64 Patcher\n";
	std::cout << "Input:  " << inputPath << "\n";
	std::cout << "Output: " << outputPath << "\n\n";

	std::ifstream input(inputPath, std::ios::binary);
	if (!input) {
		waitForEnter("Error: Cannot open " + inputPath);
		return 1;
	}

	std::vector<uint8_t> data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	input.close();

	std::cout << "Verifying...\n";
	for (const auto& p : patches) {
		if (!verify(data, p)) {
			waitForEnter("\nError: Verification failed, no patches applied.");
			return 1;
		}
	}

	std::cout << "\nApplying...\n";
	for (const auto& p : patches)
		apply(data, p);

	std::ofstream output(outputPath, std::ios::binary);
	if (!output) {
		waitForEnter("Error: Cannot create " + outputPath);
		return 1;
	}

	output.write(reinterpret_cast<const char*>(data.data()), data.size());
	output.close();

	std::cout << "\n=== Complete ===\n";
	std::cout << "Output: " << outputPath << "\n";

	waitForEnter();
	return 0;
}
