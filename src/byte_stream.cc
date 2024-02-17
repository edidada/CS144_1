#include "byte_stream.hh"

#include <cstdint>
#include <string_view>
#include <utility>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  return closed_;
}

void Writer::push( string data )
{
  if ( Writer::is_closed() or Writer::available_capacity() == 0 or data.empty() ) {
    return;
  }

  if ( data.size() > Writer::available_capacity() ) {
    data.resize( Writer::available_capacity() );
  }
  total_pushed_ += data.size();
  total_buffered_ += data.size();

  stream_.emplace( move( data ) );
  stream_view_.emplace( stream_.back() );
}

void Writer::close()
{
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - total_buffered_;
}

uint64_t Writer::bytes_pushed() const
{
  return total_pushed_;
}

bool Reader::is_finished() const
{
  return closed_ and total_buffered_ == 0;
}

uint64_t Reader::bytes_popped() const
{
  return total_popped_;
}

string_view Reader::peek() const
{
  return stream_view_.empty() ? string_view {} : stream_view_.front();
}

void Reader::pop( uint64_t len )
{
  total_buffered_ -= len;
  total_popped_ += len;
  while ( len != 0 ) {
    const uint64_t size = stream_view_.front().size();
    if ( size > len ) {
      stream_view_.front().remove_prefix( len );
      break; // with len = 0
    }
    stream_view_.pop();
    stream_.pop();
    len -= size;
  }
}

uint64_t Reader::bytes_buffered() const
{
  return total_buffered_;
}
