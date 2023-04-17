/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project
    -p, --upot=REAL   Umbral de potencia para la detección de sonoro/sordo [default: -1e6]
    -1, --ur1=REAL    Umbral de la autocorrelación de 1 para sonoro/sordo [default: 0.7]
    -m, --urmax=REAL     Umbral del maximo de la autocorrelación [default: 0.4]

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  float upot = stof(args["--upot"].asString());
  float ur1 = stof(args["--ur1"].asString());
  float urmax = stof(args["--urmax"].asString());

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::RECT, 50, 500, upot, ur1, urmax);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  /*auto max= *max_element(x.begin(), x.end());
  float threshold = 0.03*max;

  vector<float> clipped(x.size());
  clipped=x;

    for (size_t i = 0; i < x.size(); ++i) {
    if (x[i] > threshold) {
      clipped[i] = x[i] - threshold;
    } else if (x[i] < -threshold) {
      clipped[i] = x[i] + threshold;
    } else {
      clipped[i] = 0.0;
    }
  }*/
  
  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;//f0=clipped;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  /*
  vector<float> mediana;
  mediana=f0;
  mediana.begin()=f0.begin();
  for (int i = 1; i < f0.size() - 1; i++) {
    std::vector<float> values = {f0[i-1], f0[i], f0[i+1]};
    std::sort(values.begin(), values.end());
    mediana[i]=values[1];
  }
  mediana.end()=f0.end();
  */

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
