/*
 *	Copyright (c) Evan Clegern, December 2021
 *
 * 	This program is free software; you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 * 
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 * 
 * 	You should have received a copy of the GNU General Public License
 * 	along with this program.  If not, see <https://www.gnu.org/licenses/>
 * 
 */

// BEHOLD the great memory muncher
// Have you ever wanted to store a 4,194,240-bit number?
// At the cost of 4 megabytes of RAM, now you can!

#include <stdexcept>
#include <iostream>
#include <cstdint>

namespace ERCLIB {
	
typedef std::uint_fast8_t u8;
typedef std::uint_fast16_t u16;
typedef std::uint_fast64_t u64;


template<u16 cnt> class ExtendedUInt {
	//With the 16-bit max, you can technically create a 4,194,240-bit unsigned integer... if you wanted to.
	
	u64 data[cnt]; // [0] is the lowest, so it's easier for me to code :/
	
	inline void zero() noexcept {
		for (u16 i=0; i<cnt; i++) {
			this->data[i] = 0;
		}
	}
	
	inline void add(u64 num, u16 ind) {
		if (ind >= cnt) throw std::out_of_range("Cannot access a portion of ExtendedUInt outside of designated length.");
		u64 old = this->data[ind];
		this->data[ind] += num;
		if (this->data[ind] < old) {
			if (ind == cnt-1) {
				throw std::overflow_error("ExtendedUInt addition overflow.");
			} else {
				this->add(1, ind + 1);
			}
		}
	}
	//! Okay, so strange property here.
	//! If there's an underflow, it wraps back around, right?
	//! I did some testing, and the next integer after it gets -1'd, and then whatever difference is left after the wrap-around?
	//! That's that portion's data.
	//! Weird?
	//! 0x16F --> 0001 0110 1111 ; 0x7A --> 0000 0111 1010 ; 0x16F - 0x7A --> 0000 1111 0101 ; 0x6F - 0x7A [constrained] --> 1111 0101
	inline void sub(u64 num, u16 ind) {
		if (ind >= cnt) throw std::out_of_range("Cannot access a portion of ExtendedUInt outside of designated length.");
		u64 old = this->data[ind];
		this->data[ind] -= num;
		if (this->data[ind] > old) {
			if (ind == cnt-1) {
				throw std::underflow_error("ExtendedUInt addition underflow.");
			} else {
				this->sub(1, ind + 1);
			}
		}
	}
	
	inline void shiftRight(u16 bits) {
		if (bits >= 64) {
			this->zero();
		} else {
			u64 old = 0;
			for (u16 i=cnt-1; i>0; i--) {
				u64 ne = (this->data[i] >> bits) | old;
				old = (this->data[i]) << (64 - bits);
				this->data[i] = ne;
			}
			this->data[0] = (this->data[0] >> bits) | old;
		}
	};
	inline void shiftLeft(u16 bits) {
		if (bits >= 64) {
			this->zero();
		} else {
			u64 old = 0;
			for (u16 i=0; i<cnt; i++) {
				u64 ne = (this->data[i] << bits) | old;
				old = (this->data[i]) >> (64 - bits);
				this->data[i] = ne;
			}
		}
	};
	inline void multiplyFull(u64 num) {
		//Multiplication is simple.
		//Break into powers of two, like: 11 --> 8, 2, 1; 12 --> 8, 4; 21 --> 16, 4, 1
		//Essentially, each bit in the number translates to the shifts of each portion.
		//Then, add them together.
		ExtendedUInt<cnt> old(*this);
		if (!(num & 1)) {
			//Is it dumb? Yes. Is it necessary for any of the even multipliers? Yes (since we're adding to self).
			for (u16 x=cnt-1;x>0;x--) {
				this->sub(old.at(x), x);
			}
			this->sub(old.at(0),0);
		}
		for (u8 i=1; i<64; i++) {
			if ((num >> i) & 1) {
				ExtendedUInt<cnt> bruh(old);
				bruh <<= i;
				for (u16 x=0;x<cnt;x++) {
					this->add(bruh.at(x), x);
				}
			}
		}
	}
public:
	
	explicit ExtendedUInt(u64 base) noexcept {
		zero();
		data[0] = base;
	}
	explicit ExtendedUInt(u64 list[cnt]) : data(list) {}
	
	//! NOTE WITH THIS INITIALIZER: MUST BE ORGANIZED IN LITTLE-ENDIAN FORMAT, AS THE VALUES ARE COPIED DIRECTLY.
	//! PRINT() PUTS IT IN BIG-ENDIAN FOR MORE READABILITY, BUT THE DATA STORAGE IS MUCH EASIER IN LITTLE-ENDIAN.
	explicit ExtendedUInt(std::initializer_list<u64> vals) noexcept {
		auto list = std::data(vals);
		
		u8 nu = vals.size();
		
		if (nu >= cnt) { //trim or otherwise copy
			for (u16 i=0; i<cnt; i++) {
				data[i] = list[i];
			}
		} else { //zero the rest
			for (u16 i=0; i<nu; i++) {
				data[i] = list[i];
			}
			for (u16 i=nu; i<cnt; i++) {
				data[i] = 0;
			}
		}
	}
	ExtendedUInt(const ExtendedUInt<cnt>& b) {
		for (u16 i=0;i<cnt;i++) {
			data[i] = b.at(i);
		}
	}
	inline void operator=(ExtendedUInt<cnt> b) {
		for (u16 i=0;i<cnt;i++) {
			this->data[i] = b.at(i);
		}
	}
	
	inline const u64& at(u16 index) const noexcept {
		return this->data[index];
	}
	
	void operator+=(ExtendedUInt<cnt> b) {
		for (u16 i=0;i<cnt;i++) {
			this->add(b.at(i), i);
		}
	}
	void operator+=(u64 b) {
		this->add(b, 0);
	}
	
	void operator-=(ExtendedUInt<cnt> b) {
		for (u16 i=cnt-1; i>0; i--) {
			this->sub(b.at(i), i);
		}
		this->sub(b.at(0), 0);
	}
	void operator-=(u64 b) {
		this->sub(b, 0);
	}
	
	void operator*=(u64 b) {
		this->multiplyFull(b);
	}
	
	void operator<<=(u16 b) {
		this->shiftLeft(b);
	}
	void operator>>=(u16 b) {
		this->shiftRight(b);
	}
	
	void operator^=(ExtendedUInt<cnt> b) {
		for (u16 i=0; i<cnt; i++) {
			this->data[i] ^= b.at(i);
		}
	}
	void operator|=(ExtendedUInt<cnt> b) {
		for (u16 i=0; i<cnt; i++) {
			this->data[i] |= b.at(i);
		}
	}
	void operator&=(ExtendedUInt<cnt> b) {
		for (u16 i=0; i<cnt; i++) {
			this->data[i] &= b.at(i);
		}
	}
	
	void print() const noexcept {
		for (u16 i = cnt - 1; i > 0; i--) {
			std::cout << this->data[i] << ' ';
		}
		std::cout << this->data[0] << '\n';
	}
	
	void operator++() {
		this->add(1, 0);	
	}
	void operator--() {
		this->sub(1, 0);	
	}
};

typedef ExtendedUInt<2> u128;
typedef ExtendedUInt<3> u192;
typedef ExtendedUInt<4> u256;
typedef ExtendedUInt<5> u320;

}
