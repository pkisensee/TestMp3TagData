///////////////////////////////////////////////////////////////////////////////
//
//  TestMp3TagData.cpp
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.
//
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "File.h"
#include "Log.h"
#include "Mp3AudioData.h"
#include "Mp3TagData.h"

extern "C" { __declspec( dllimport ) void __stdcall DebugBreak(); }

// Macros
#ifdef _DEBUG
#define test(e)   assert(e)
#else
#define test(e)   static_cast<void>( (e) || ( DebugBreak(), 0 ) )
#endif

using namespace PKIsensee;
namespace fs = std::filesystem;

void TestBaseTagData()
{
  test( Mp3BaseTagData::GetMaxGenre() > 0 );
  test( std::string( Mp3BaseTagData::GetGenre( 0 ) ) == std::string( "Blues" ) );

  uint8_t zero = 0;
  uint8_t badFrameHdr[ 5 ] = "0aX:";
  test( !Mp3BaseTagData::IsValidFrame( nullptr ) );
  test( !Mp3BaseTagData::IsValidFrame( &zero ) );
  test( !Mp3BaseTagData::IsValidFrame( badFrameHdr ) );
  test( !Mp3BaseTagData::IsValidFrameID( "tooLong" ) );
  test( !Mp3BaseTagData::IsValidFrameID( "sm" ) );
  test( Mp3BaseTagData::IsValidFrameID( "TPE1" ) );

  test( Mp3BaseTagData::IsTextFrame( Mp3FrameType::Artist ) );
  test( Mp3BaseTagData::IsTextFrame( std::string( "TCOM" ) ) );
  test( Mp3BaseTagData::IsTextFrame( "TALB" ) );
  test( Mp3BaseTagData::GetFrameType( "TCON" ) == Mp3FrameType::Genre );
  test( Mp3BaseTagData::GetFrameType( "TCon" ) == Mp3FrameType::None );
}

void TestTagData()
{
  // Validate expected data
  fs::path mp3( "MP3\\test.mp3" );
  Mp3TagData tag;
  test( tag.LoadTagData( mp3 ) );
  test( tag.GetText( Mp3FrameType::Title ) == "Title" );
  test( tag.GetText( Mp3FrameType::Subtitle ) == "" );
  test( tag.GetText( Mp3FrameType::Genre ) == "Genre" );
  test( tag.GetText( Mp3FrameType::Artist ) == "Artist" );
  test( tag.GetText( Mp3FrameType::Album ) == "Album" );
  test( tag.GetText( Mp3FrameType::Composer ) == "Composer" );
  test( tag.GetText( Mp3FrameType::Orchestra ) == "Orchestra" );
  test( tag.GetText( Mp3FrameType::OrigArtist ) == "OrigArtist" );
  test( tag.GetText( Mp3FrameType::Year ) == "1234" );
  test( tag.GetText( Mp3FrameType::OrigYear ) == "4321" );
  test( tag.GetText( Mp3FrameType::TrackNum ) == "333" );
  test( tag.GetText( Mp3FrameType::BeatsPerMinute ) == "123" );
  test( tag.GetText( Mp3FrameType::Duration ) == "219167" );
  test( tag.GetText( Mp3FrameType::Key ) == "C#m" );
  test( tag.GetText( Mp3FrameType::Conductor ) == "Conductor" );
  test( tag.GetText( Mp3FrameType::Language ) == "eng" );
  test( tag.GetText( Mp3FrameType::Mood ) == "Wild" );
  test( tag.GetCommentCount() == 2 );
  test( tag.GetComment( 0 ) == "Comment" );
  test( tag.GetComment( 1 ) == "Wild" );
  //test( tag.GetComment( 2 ) == "" ); // doesn't exist; assert
  test( !tag.IsDirty() );

  // Make a copy and validate correctness
  fs::path mp3Copy( "MP3\\testCopy.mp3" );
  test( fs::copy_file( mp3, mp3Copy, fs::copy_options::overwrite_existing ) );
  Mp3TagData tagCopy;
  test( tagCopy.LoadTagData( mp3Copy ) );
  for( Mp3FrameType frameType = Mp3FrameType::None; frameType != Mp3FrameType::Max; ++frameType )
  {
    if( Mp3BaseTagData::IsTextFrame( frameType ) )
      test( tag.GetText( frameType ) == tagCopy.GetText( frameType ) );
  }
  test( tag.GetComment( 0 ) == tagCopy.GetComment( 0 ) );
  test( !tagCopy.Write() );

  // Modify the copy; some strings are shorter, some longer, some unchanged
  tagCopy.SetText( Mp3FrameType::Title, "T" );
  test( tagCopy.IsDirty() );
  tagCopy.SetText( Mp3FrameType::Subtitle, "NewSubtitle" );
  tagCopy.SetText( Mp3FrameType::Genre, "GenreCopy" );
  tagCopy.SetText( Mp3FrameType::Artist, "ArtistCopy" );
  tagCopy.SetText( Mp3FrameType::Album, "AlbumCopy" );
  tagCopy.SetText( Mp3FrameType::Composer, "Comp" );
  tagCopy.SetText( Mp3FrameType::Orchestra, "Orch" );
  tagCopy.SetText( Mp3FrameType::OrigArtist, "OrigArt" );
  tagCopy.SetText( Mp3FrameType::Year, "1234" );
  tagCopy.SetText( Mp3FrameType::TrackNum, "15/325" );
  tagCopy.SetText( Mp3FrameType::BeatsPerMinute, "1" );
  tagCopy.SetText( Mp3FrameType::Duration, "9923821" );
  tagCopy.SetText( Mp3FrameType::Conductor, "Cond" );
  tagCopy.SetText( Mp3FrameType::Language, "est" );
  tagCopy.SetText( Mp3FrameType::Mood, "Crazy" );

  test( tagCopy.GetText( Mp3FrameType::Title ) == "T" );
  test( tagCopy.GetText( Mp3FrameType::Subtitle ) == "NewSubtitle" );
  test( tagCopy.GetText( Mp3FrameType::Genre ) == "GenreCopy" );
  test( tagCopy.GetText( Mp3FrameType::Artist ) == "ArtistCopy" );
  test( tagCopy.GetText( Mp3FrameType::Album ) == "AlbumCopy" );
  test( tagCopy.GetText( Mp3FrameType::Composer ) == "Comp" );
  test( tagCopy.GetText( Mp3FrameType::Orchestra ) == "Orch" );
  test( tagCopy.GetText( Mp3FrameType::OrigArtist ) == "OrigArt" );
  test( tagCopy.GetText( Mp3FrameType::Year ) == "1234" );
  test( tagCopy.GetText( Mp3FrameType::OrigYear ) == "4321" );
  test( tagCopy.GetText( Mp3FrameType::TrackNum ) == "15/325" );
  test( tagCopy.GetText( Mp3FrameType::BeatsPerMinute ) == "1" );
  test( tagCopy.GetText( Mp3FrameType::Duration ) == "9923821" );
  test( tagCopy.GetText( Mp3FrameType::Key ) == "C#m" );
  test( tagCopy.GetText( Mp3FrameType::Conductor ) == "Cond" );
  test( tagCopy.GetText( Mp3FrameType::Language ) == "est" );
  test( tagCopy.GetText( Mp3FrameType::Mood ) == "Crazy" );

  tagCopy.SetComment( 0, "CommentCopy" );
  test( tagCopy.GetComment( 0 ) == "CommentCopy" );

  // Nuke some existing and non-existent frames
  tagCopy.SetText( Mp3FrameType::OrigArtist, "" );
  tagCopy.SetText( Mp3FrameType::OrigArtist, "" );
  tagCopy.SetText( Mp3FrameType::OrigArtist, "OrigArtist" );
  tagCopy.SetText( Mp3FrameType::BeatsPerMinute, "" );
  tagCopy.SetComment( 0, "CommentCopy" );
  tagCopy.SetComment( 1, "" );
  tagCopy.SetComment( 1, "RestoreComment1" );

  // Write the new file
  test( tagCopy.IsDirty() );
  test( tagCopy.Write() );

  // Read the file back and validate
  Mp3TagData tagRead;
  test( tagRead.LoadTagData( mp3Copy ) );
  for( Mp3FrameType frameType = Mp3FrameType::None; frameType != Mp3FrameType::Max; ++frameType )
  {
    if( Mp3BaseTagData::IsTextFrame( frameType ) )
    {
      switch( frameType )
      {
      case Mp3FrameType::OrigArtist:     test( tagRead.GetText( frameType ) == "OrigArtist" ); break;
      case Mp3FrameType::BeatsPerMinute: test( tagRead.GetText( frameType ) == "" ); break;
      default:                           test( tagRead.GetText( frameType ) == tagCopy.GetText( frameType ) ); break;
      }
    }
  }
      
  test( tagRead.GetCommentCount() == 2 );
  test( tagRead.GetComment( 0 ) == "CommentCopy" );
  test( tagRead.GetComment( 1 ) == "RestoreComment1" );

  // Force a full file rewrite
  std::string huge( 10000, 'X' );
  tagCopy.SetComment( 0, huge );
  test( tagCopy.IsDirty() );
  test( tagCopy.Write() );

  // Read the file back and validate
  Mp3TagData tagRead2;
  test( tagRead2.LoadTagData( mp3Copy ) );
  for( Mp3FrameType frameType = Mp3FrameType::None; frameType != Mp3FrameType::Max; ++frameType )
  {
    if( Mp3BaseTagData::IsTextFrame( frameType ) )
      test( tagRead2.GetText( frameType ) == tagRead.GetText( frameType ) );
  }
  test( tagRead2.GetComment( 0 ) == huge );

  // File cleanup
  File( mp3Copy ).Delete();
}

class ForEachSong
{
public:

  ForEachSong() {
    ++frameType_;
  }
  void operator()( const fs::path& ) const;

  mutable size_t maxFrames = 0;

private:

  // pseudo-random frame generator
  mutable Mp3FrameType frameType_ = Mp3FrameType::None;

};

///////////////////////////////////////////////////////////////////////////////
//
// Called for each song

void ForEachSong::operator()( const fs::path& path ) const
{
  static fs::path lastDir;
  fs::path dir = path.parent_path();
  if( dir != lastDir )
  {
    std::cout << "\n" << dir.relative_path().string();
    lastDir = dir;
  }
  std::cout << ".";

  // Skip any files that already contain "Copy"
  std::string fileName = path.stem().string();
  if( fileName.find( "Copy" ) != std::string::npos )
    return;

  // Copy the file
  fs::path fileCopy( path );
  fileCopy.replace_filename( fileName + "Copy" + path.extension().string() );
  test( fs::copy_file( path, fileCopy, fs::copy_options::overwrite_existing ) );

  // Change the file; randomly do comments
  Mp3TagData tag;
  if( !tag.LoadTagData( fileCopy ) )
  {
    test( File( fileCopy ).Delete() ); // remove temporary file
    return; // skip invalid files
  }

  // Count frames
  maxFrames = std::max( maxFrames, tag.GetFrameCount() );

  std::string newText( "Some new text for this frame" );
  std::string hugeText( 10000, 'H' );
  std::string emptyText( "" );
  std::string newComment( "Some new or updated comments for this frame" );
  switch( fileName[ 0 ] % 4 )
  {
  case 0: tag.SetText( frameType_, emptyText ); break;
  case 1: tag.SetText( frameType_, hugeText ); break;
  default: tag.SetText( frameType_, newText ); break;
  }
  size_t comment = size_t(-1);
  if( fileName[ 1 ] % 2 )
  {
    auto commentCount = tag.GetCommentCount();
    if( commentCount )
    {
      comment = ( fileName[ 2 ] % commentCount ) + 1;
      tag.SetComment( comment, newComment );
    }
  }

  // Read the MPEG data
  Mp3AudioData audio;
  test( audio.Load( fileCopy ) );
  test( audio.HasMpegAudio() );
  test( audio.GetVersion() != MpegVersion::None );
  test( audio.GetLayer() != MpegLayer::None );
  test( audio.GetDurationMs() > 0u );
  test( audio.GetDurationMs() < 60 * 60 * 1000 ); // less than 60 minutes
  test( audio.GetFrameCount() > 1 );

  if( tag.IsDirty() ) // it's possible we've deleted a non-existant frame, in which case there's nothing to write
    test( tag.Write() );

  Mp3TagData tagRead;
  test( tagRead.LoadTagData( fileCopy ) );
  for( Mp3FrameType frameType = Mp3FrameType::None; frameType != Mp3FrameType::Max; ++frameType )
  {
    if( Mp3BaseTagData::IsTextFrame( frameType ) )
    {
      if( frameType == frameType_ )
      {
        switch( fileName[ 0 ] % 4 )
        {
        case 0: test( tagRead.GetText( frameType_ ) == emptyText ); break;
        case 1: test( tagRead.GetText( frameType_ ) == hugeText ); break;
        default: test( tagRead.GetText( frameType_ ) == newText ); break;
        }
      }
      else
      {
        test( tagRead.GetText( frameType ) == tag.GetText( frameType ) );
      }
    }
  }
  for( auto i = 0u; i < tagRead.GetCommentCount(); ++i )
  {
    if( comment == i )
      test( tagRead.GetComment( i ) == newComment );
    else
      test( tagRead.GetComment( i ) == tag.GetComment( i ) );
  }

  test( !tag.IsDirty() );
  test( File( fileCopy ).Delete() ); // comment out this line to leave the temporary files behind for further investigation
  ++frameType_;
  if( frameType_ == Mp3FrameType::Comment )
  {
    frameType_ = Mp3FrameType::None; // wrap
    ++frameType_;
  }
}

void TestMultiFile()
{
  ForEachSong forEachSong;
  for( const auto& entry : fs::directory_iterator( ".\\MP3" ) )
  {
    fs::path path = entry.path();
    if( path.extension() == ".mp3" )
      forEachSong( path );
  }
  PKLOG_SCRN( "\nMaximum frame count for MP3 test files: %u\n", forEachSong.maxFrames );
}

void TestAudioData()
{
  fs::path mp3( "MP3\\BadTLEN.mp3" );
  Mp3TagData tag;
  test( tag.LoadTagData( mp3 ) );
  Mp3AudioData audio;
  test( audio.Load( mp3 ) );
  auto duration = audio.GetDurationMs();
  test( duration == 346279u );

  Mp3AudioData audio2;
  test( audio2.Load( mp3, tag.GetAudioBufferOffset() ) );
  auto duration2 = audio2.GetDurationMs();
  test( duration == duration2 );

  fs::path mp3bad( "MP3\\BadHdr.mp3" );
  Mp3TagData tagBad;
  test( !tagBad.LoadTagData( mp3bad ) );
}

int __cdecl main( int, char** )
{
  TestBaseTagData();
  TestAudioData();
  TestTagData();
  TestMultiFile();
  TestAudioData();
  return 0;
}
