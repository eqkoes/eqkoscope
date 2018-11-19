/* 
   A very small MIDI file reader (SMF). It will read any MIDI file into a MidiSequence structure, 
   that is just a list of MIDI events, that are ordered timestamped (tempo change events are handled).

   license: Public Domain 
*/
#pragma once

#include <stdint.h>
#include <cassert>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

inline bool readWholeFileContent(const std::string &filename, std::vector<uint8_t> &data) {
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);
  data.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
  return f.good();
}

/** hold the MIDI file content, as a list of events */
struct MidiSequence {
  struct Event {
    size_t tick; // the event position, in term of ticks
    double when; // the timestamp, in seconds
    uint16_t track; // track number for that event
    std::vector<uint8_t> midi; // the event raw bytes
    Event() : tick(0), when(0), track(0) {}
    bool operator<(const Event &other) const { return tick < other.tick; }
  };
  std::vector<Event> evts;
  double sequenceDuration() const { return evts.empty() ? 0 : evts.back().when; }
};

/** read any MIDI file into a MidiSequence */
struct SimpleMidiReader {
  std::vector<uint8_t> data;
  uint8_t *ptr, *end;
  uint16_t format, nb_tracks, division;

  SimpleMidiReader() {}

  /* read the content of the specified file */
  bool read(const std::string &filename, MidiSequence &seq) {
    seq.evts.clear();
    if (!readWholeFileContent(filename, data) || data.empty()) return false;
    ptr = &data[0];
    end = ptr + data.size();
    uint32_t length;
    uint32_t track_nb_bytes;
    if (!rdID("MThd") != 0 ||
        !rd<uint32_t>(length) ||
        length != 6 ||
        !rd<uint16_t>(format) ||
        !rd<uint16_t>(nb_tracks) ||
        !rd<uint16_t>(division)) {
      std::cerr << "not a MIDI file"; return false;
    }
    size_t tick_position = 0;
    for (uint16_t track = 0; track < nb_tracks; ++track) {
      if (!rdID("MTrk") || !rd<uint32_t>(track_nb_bytes)) {
        std::cerr << "could not read track" << track+1 << "\n"; return false;
      }
      uint8_t *end_of_track = ptr + track_nb_bytes; assert(end_of_track <= end);
      if (format != 2) tick_position = 0;
      uint8_t running_status = 0;
      while (ptr < end_of_track) {
        uint32_t delta_tick = 0;
        uint32_t len = 0;
        uint8_t code;
        if (!readVariableLength(delta_tick)) break; // end of track...

        uint8_t *evt_start = ptr;
        if (!rd<uint8_t>(code)) return false;
        tick_position += delta_tick;

        MidiSequence::Event evt;
        switch (code) {
          case 0xFF: { // smf meta event
            skipBytes(1);
            readVariableLength(len);
            skipBytes(len);
          } break;
          case 0xF0:
          case 0xF7: { // sysex
            readVariableLength(len);
            skipBytes(len);
          } break;
          default: {
            if (code & 0x80) {
              running_status = code;
              skipBytes(1);
            } else {
              code = running_status;
              evt.midi.push_back(running_status);
            }
            code &= 0xF0;
            skipBytes((code == 0xC0 || code == 0xD0) ? 0u : 1u);
          } break;
        }
        evt.tick = tick_position;
        evt.track = track;
        evt.when = 0;
        evt.midi.insert(evt.midi.end(), evt_start, ptr);
        seq.evts.push_back(evt);
      }
    }
    // merge the tracks
    std::sort(seq.evts.begin(), seq.evts.end());
    // put the correct timestamps
    applyTempoEvents(seq);
    return true;
  }

  void applyTempoEvents(MidiSequence &seq) {
    double tick_duration = 0.5 / (double)division;
    double t_prev = 0;
    size_t tick_prev = 0;
    for (size_t i=0; i < seq.evts.size(); ++i) {
      MidiSequence::Event &evt = seq.evts[i];
      evt.when = (evt.tick - tick_prev)*tick_duration + t_prev;
      t_prev = evt.when; tick_prev = evt.tick;
      if (evt.midi.size() == 6 && evt.midi[0] == 0xFF && evt.midi[1] == 0x51 && evt.midi[2] == 3) {
        unsigned value = (evt.midi[3] << 16) + (evt.midi[4] << 8) + evt.midi[5];
        tick_duration = (double) (0.000001 * value / (double)division);
      }
    }
  }

  template <typename T> bool rd(T &v) {
    if (ptr + sizeof(T) > end) { std::cerr << "incomplete MIDI file\n"; return false; }
    v = 0;
    for (size_t k=0; k < sizeof(T); ++k, ++ptr) {
      v = (v << 8) | *ptr;
    }
    return true;
  }

  bool rdID(const char *id) {
    if (ptr + strlen(id) > end) return false;
    if (strncmp((const char*)ptr, id, strlen(id)) != 0) return false;
    ptr += strlen(id);
    return true;
  }

  bool readVariableLength(uint32_t &v) {
    v = 0;
    unsigned char c;
    do {
      if (ptr >= end) return false;
      c = *ptr++;
      v = (v << 7) + (c & 0x7f);
    } while (c & 0x80);
    return true;
  }

  void skipBytes(size_t nb) {
    ptr += nb; if (ptr > end) ptr = end;
  }
};

/*
  build the test app with something like:
  g++ -DTEST_MIDI_READER -Wall -W -O2 -x c++ -o simple_midi_reader simple_midi_reader.hh
  cl /O2 -DTEST_MIDI_READER /EHsc /Tp simple_midi_reader.hh
*/
#ifdef TEST_MIDI_READER 
#include <cstdio>
// read the files from the command line and display their content
int main(int argc, char **argv) {
  SimpleMidiReader smr;
  MidiSequence seq;
  for (int i=1; i < argc; ++i) {
    std::cout << "Reading file: " << argv[i] << std::endl;
    if (smr.read(argv[i], seq)) {
      std::cout << " format: " << smr.format << ", nb_tracks=" << smr.nb_tracks << "\n";
      for (size_t j=0; j < seq.evts.size(); ++j) {
        const MidiSequence::Event &ev = seq.evts[j];
        printf("tick %5d track %d %8.4fs ", (int)ev.tick, (int)ev.track, ev.when);
        for (size_t k=0; k < ev.midi.size(); ++k) {
          printf(" %02x", ev.midi[k]);
        }
        printf("\n");
      }
    }
  }
  return 0;
}
#endif
