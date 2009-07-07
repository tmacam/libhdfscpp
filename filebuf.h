#ifndef __FILEBUF_H
#define __FILEBUF_H

#include <stdexcept>
#include <string>

namespace tmacam {

/**@brief A string-io like class, i.e., bound-checked array read and seek.
 *
 * The idea of this class is to provide a safe way to use a buffer/array/string
 * as if it was a file: you can read(), seek() and etc on it. On the same
 * time, it also wants to pretend to be a pointer/iterator: you can
 * dereference it, add  to it. All those operations are bound checked.
 */

struct filebuf {
	const char* current; /**< current position */
	const char* start; /**< start of this buffer (for seek...) */
	const char* end; /**< end of the buffer (1 past the end) */

	filebuf() : current(0), start(0),end(0){}
	filebuf(const char* buffer, unsigned int length) : current(buffer),
						     start(buffer),
						     end(buffer+length){}

	filebuf(const filebuf& orig): current(orig.current),
						     start(orig.current),
						     end(orig.end){}
	
#ifdef __MEMBUF_F_
	filebuf(const membuf& buffer) : current((const char*)buffer.start),
			start((const char*) buffer.start),
			end((const char*)buffer.start + buffer.length()){}
#endif // __MEMBUF_F_

	/**@brief Garantees you can read "length" bytes from the filebuf
	 *        and advances the current position by the same ammount.
	 *
	 * This function tries to garantee that at least length bytes from the
	 * filebuf can be read without transpessing its end. If it is possible,
	 * it will return the current buffer read position and advance its
	 * current position internally in length bytes.
	 *
	 * After a read the current position can reach the end of this filebuf
	 * and leave this filebuf in a eof state  but if it goes beyond the
	 * end then it is an error and a std::out_of_range exception will be
	 * raised
	 */
	inline const char* read(unsigned int length)
	{
		const char* old_pos = this->current;

		/* This read can make the current position reach the
		 * end of this filebuf...
		 */
		if ( (this->current + length) > this->end ){
			/* ... but if it goes *beyond* the end of the buffer
			 * it is an ERROR.
			 */
			throw std::out_of_range("End of filebuf reached or access out of bounds.");
		} else {
			this->current += length;
			return old_pos;
		}
	}

	/**@brief Seeks to a different position in the buffer.
	 *
	 * It the seek crosses the filebuf boundaries a std::out_of_range is
	 * thrown.
	 */
	const char* seek(int offset)
	{
		const char* new_pos = this->current + offset;

		if (new_pos >= this->start && new_pos < this->end){
			this->current = new_pos;
			return new_pos;
		} else {
			throw std::out_of_range("Seek crosses filebuf boundaries.");
		}

	}


	inline filebuf readf(unsigned int length)
	{
		return filebuf(this->read(length),length);
	}

	inline bool eof() const {return (this->current >= this->end);}

	inline size_t len() const {return (this->end - this->current);}


	filebuf& operator+=(unsigned int offset)
	{
		this->read(offset);
		return *this;
	}

	inline filebuf& operator++()
	{
		this->read(1);
		return *this;
	}


	/**Get the current character
	 *
	 *@throw std::out_of_range if there is no data to read or if we
	 			   reach the end of the buffer.
	 */
	inline const char operator*()
	{
		if (eof()) {
			throw std::out_of_range("End of filebuf reached"
			" or access out of bounds.");
		} else {
			return *current;
		}
	}

	/**Convert this buffer into a std::string*/
	inline std::string str() const
	{
		return std::string(current, len());
	}

};

} //namespace tmacam

#endif /* __FILEBUF_H */
// vim:syn=cpp.doxygen:autoindent:smartindent:fo+=tcroq:
