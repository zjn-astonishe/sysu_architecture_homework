#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <memory>

constexpr uint64_t initial_pc = 0x2000'00e8;
constexpr uint64_t initial_gp = 0x0001'0000;

auto read() {
	std::array<std::vector<std::string>, 3> ret;
	std::string buf;
	while( std::getline( std::cin, buf ) && buf != "# Global objects" ) {
		ret[0].push_back( buf );
	}
	while( std::getline( std::cin, buf ) && buf != "# Alias List" ) {
		ret[1].push_back( buf );
	}
	while( std::getline( std::cin, buf ) ) {
		ret[2].push_back( buf );
	}
	return ret;
}

auto split_label( const std::string& str ) {
	std::array<std::string, 2> ret;
	std::istringstream istr( str );
	istr >> ret[0] >> ret[1];
	return ret;
}

struct Content {
	std::string type;
	std::string data;
	std::unique_ptr<Content> base;
	uint64_t offset;
	Content( std::string type, std::string data, std::unique_ptr<Content> base, uint64_t offset )
		: type(std::move(type)), data(std::move(data)), base(std::move(base)), offset(offset) {}
};

// maybe nullptr
std::unique_ptr<Content> getContent( std::istringstream& istr ) {
	std::string ty;
	if( istr >> ty ) {
		if( ty != "gep32" ) {
			std::string content;
			istr >> content;
			return std::make_unique<Content>( std::move(ty), std::move(content), nullptr, 0 );
		} else {
			std::string buf;
			istr >> buf;
			assert( buf == "(" );
			auto&& base = getContent( istr );
			istr >> buf;
			assert( buf == "+" );
			uint64_t offset;
			istr >> offset;
			istr >> buf;
			assert( buf == ")" );
			return std::make_unique<Content>( std::move(ty), "", std::move(base), offset );
		}
	} else {
		return nullptr;
	}
}

auto split_global( const std::string& str ) {
	std::pair<std::string, std::vector<std::unique_ptr<Content>>> ret;
	std::istringstream istr( str );
	istr >> ret.first;
	while( auto&& content = getContent( istr ) ) {
		ret.second.push_back( std::move(content) );
	}
	return ret;
}

uint64_t size_of( const std::string& str ) {
	if( str ==    "i1" ) { return 1; }
	if( str ==    "i8" ) { return 1; }
	if( str ==   "i16" ) { return 2; }
	if( str ==   "i32" ) { return 4; }
	if( str ==   "f32" ) { return 4; }
	if( str ==   "p32" ) { return 8; }
	if( str == "gep32" ) { return 8; }
	if( str ==   "i64" ) { return 8; }
	if( str ==   "f64" ) { return 8; }
	std::cerr << "unknown type: " << str << std::endl;
	exit(1);
}

void output_value( uint64_t val, std::size_t size ) {
	if( size == 1 ) {
		std::cout << val%0x100 << " ";
	} else {
		assert( size % 2 == 0 );
#ifdef BIGENDIAN_STATIC_OBJECT
		output_value( val >> (size*4), size/2 );
		output_value( val, size/2 );
#else
		output_value( val, size/2 );
		output_value( val >> (size*4), size/2 );
#endif
	}
}

std::map<std::string, uint64_t> pc_table;
std::map<std::string, uint64_t> global_table { { "nullptr", 0 } };
std::map<std::string, std::string> alias_table;

uint64_t getPC( const std::string& str ) {
	if( str.substr( 0, 9 ) == "Function_" ) {
		const auto is_alias = alias_table.count( str.substr(9) );
		if( is_alias ) {
			return getPC( "Function_" + alias_table[str.substr(9)] );
		}
	}
	try {
		return pc_table.at( str );
	} catch( const std::out_of_range& err ) {
		if( str.substr( 0, 9 ) == "Function_" ) {
			std::cerr << " *** linker error: cannot find the function '" << str.substr( 9 ) << "'." << std::endl;
		} else {
			std::cerr << " *** linker error: cannot find the basic block '" << str << "'." << std::endl;
		}
		std::exit(1);
	}
}

uint64_t getAddress( const std::string& str ) {
	try {
		return global_table.at( str );
	} catch( const std::out_of_range& err ) {
		std::cerr << " *** linker error: cannot find the variable '" << str << "'." << std::endl;
		std::exit(1);
	}
}

int64_t relAddr( const std::string& target, uint64_t instr_pc ) {
	return (int64_t)( getPC( target ) - instr_pc );
}

std::string getLoadSomeAddressInstr( const std::string& str, char dst, uint64_t instr_pc ) {
	const auto is_func = pc_table.count( "Function_" + str );
	const auto is_global_var = global_table.count( str );
	const auto is_alias = alias_table.count( str );
	if( !is_func && !is_global_var ) {
		if( is_alias ) {
			return getLoadSomeAddressInstr( alias_table[str], dst, instr_pc );
		}
		std::cerr << " *** linker error: cannot find the target '" << str << "'." << std::endl;
		std::exit(1);
	} else if( is_func ) {
		uint64_t rel_addr = relAddr( "Function_" + str, instr_pc );
		uint64_t upper = (rel_addr+0x800) & 0xFFFF'FFFF'FFFF'F000;
		int64_t lower = rel_addr - upper;
		std::ostringstream ostr;
		ostr << "AUiPC    " << dst << ", " << std::left << std::setw( 17 ) << (int64_t)upper / (1ll<<12) << '\n';
		ostr << " [......] ";
		ostr << "ADDi.64  " << dst << ", " << dst << "0, " << std::left << std::setw( 17 ) << lower;
		return ostr.str();
	} else if( is_global_var ) {
		uint64_t abs_addr = global_table.at( str );
		uint64_t upper = (abs_addr+0x800) & 0xFFFF'FFFF'FFFF'F000;
		int64_t lower = abs_addr - upper;
		std::ostringstream ostr;
		ostr << "LUi      " << dst << ", " << std::left << std::setw( 17 ) << (int64_t)upper / (1ll<<12) << '\n';
		ostr << " [......] ";
		ostr << "ADDi.64  " << dst << ", " << dst << "0, " << std::left << std::setw( 17 ) << lower;
		return ostr.str();
	} else {
		std::cerr << " *** linker error: the symbol '" << str << "' is both function and variable." << std::endl;
		std::exit(1);
	}
}

uint64_t getValue( const std::unique_ptr<Content>& content ) {
	if( content->type == "p32" ) {
		const auto is_func = pc_table.count( "Function_" + content->data );
		const auto is_global_var = global_table.count( content->data );
		if( !is_func && !is_global_var ) {
			std::cerr << " *** linker error: cannot find the target '" << content->data << "'." << std::endl;
			std::exit(1);
		} else if( is_func ) {
			return getPC( "Function_" + content->data );
		} else if( is_global_var ) {
			return getAddress( content->data );
		} else {
			std::cerr << " *** linker error: the symbol '" << content->data << "' is both function and variable." << std::endl;
			std::exit(1);
		}
	} else if( content->type == "gep32" ) {
		return getValue( content->base ) + content->offset;
	} else {
		return std::stoull( content->data );
	}
}

std::string spliceTarget( std::size_t n, const std::string& code ) {
	return code.substr( n, code.find_first_of( " #", n ) - n );
}

bool matchTarget( const std::string& code, std::size_t start_pos, const std::string& str ) {
	const auto size = str.size();
	if( code.size() < start_pos + size ) {
		return false;
	}
	else {
		return code.substr( start_pos, size ) == str;
	}
}

int main() {
	const auto [codes, global, aliases] = read();
	{
		uint64_t pc = initial_pc;
		for( const auto& code : codes ) {
			if( code[0] != '[' && code[0] != ' ' ) {
				const auto [s1, s2] = split_label( code );
				s2 != ":" && std::cerr << code << '\n';
				assert( s2 == ":" );
				pc_table[s1] = pc;
			} else {
				if( matchTarget( code, 10, "Global" ) ) {
					pc += 8;
				} else {
					pc += 4;
				}
			}
		}
	}

	{
		uint64_t global_address = initial_gp;
		for( const auto& global_val : global ) {
			const auto [id, vec] = split_global( global_val );
			global_table[id] = global_address;
			for( const auto& content : vec ) {
				const uint64_t size = size_of( content->type );
				// padding
				for( ; global_address % size != 0; ) {
					++global_address;
				}
				global_address += size;
			}
			// 16byte align
			while( global_address % 16 ) {
				++global_address;
			}
		}
	}

	{
		for( const auto& alias : aliases ) {
			const auto [id, vec] = split_global( alias );
			assert( vec.size() == 1 );
			assert( vec[0]->base == nullptr );
			alias_table[id] = vec[0]->data;
		}
	}

	{
		uint64_t pc = initial_pc;
		for( const auto& code : codes ) {
			if( code[0] != ' ' ) {
				std::cout << code << '\n';
			} else {
				if( matchTarget( code, 10, "Global" ) ) {
					const auto target = spliceTarget( 22, code );
					const auto loadImmInstr = getLoadSomeAddressInstr( target, code[19], pc );
					std::cout << code.substr(0, 10) << loadImmInstr << "#" << code.substr(8) << '\n';
					pc += 4; // 2命令分のスペースをとるので追加分
				} else if( matchTarget( code, 22, "BB_" ) ) {
					const auto target = spliceTarget( 22, code );
					std::cout << code.substr(0, 22) << relAddr( target, pc ) / 4 << '\n';
				} else if( matchTarget( code, 22, "Function_" ) ) {
					const auto target = spliceTarget( 22, code );
					std::cout << code.substr(0, 22) << relAddr( target, pc ) / 4 << '\n';
				} else if( matchTarget( code, 32, "BB_" ) ) {
					const auto target = spliceTarget( 32, code );
					std::cout << code.substr(0, 32) << relAddr( target, pc ) / 4 << '\n';
				} else {
					std::cout << code << '\n';
				}
				pc += 4;
			}
		}
	}

	std::cout << "Initialize values" << std::endl;

	{
		uint64_t global_address = initial_gp;
		for( const auto& str : global ) {
			const auto [id, vec] = split_global( str );
			for( const auto& content : vec ) {
				const uint64_t val = getValue( content );
				const uint64_t size = size_of( content->type );
				// padding
				for( ; global_address % size != 0; ) {
					output_value( 0, 1 );
					++global_address;
				}
				output_value( val, size );
				global_address += size;
			}
			// 16byte align
			while( global_address % 16 ) {
				++global_address;
				std::cout << 0 << " ";
			}
			// 一行が長すぎるとアセンブラが読み込めない
			if( !vec.empty() ) {
				std::cout << '\n';
			}
		}
	}

	return 0;
}
