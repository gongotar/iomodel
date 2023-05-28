// Copyright 2023 Zuse Institute Berlin
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.


 //
// Created by Masoud Gholami on 13.05.22.
//

#ifndef EVALUATION_UTILS_HPP
#define EVALUATION_UTILS_HPP

#include <vector>
#include <numeric>
#include <sys/wait.h>

namespace measurement {

    class utils {

    public:

        template <typename T>
        static std::pair <double, double> linear_regression (std::vector<T> x_set, std::vector<T> y_set) {

            std::ofstream data {"tmp_data"};

            for (long i = 0; i < x_set.size(); ++i) {
                data << std::fixed << x_set.at (i) << "\t" << y_set.at (i) << std::endl;
            }

            data.flush();

            auto pid = fork ();
            assert (pid >= 0);
            if (pid) {
                int status;
                wait (&status);
                assert (WIFEXITED (status));
                assert (WEXITSTATUS (status) == EXIT_SUCCESS);
            } else {
                execlp ("./regression.py", "regression.py", nullptr);
                perror ("Exec error");
                exit (EXIT_FAILURE);
            }

            std::ifstream result {"py_out"};

            double slope, intercept;

            result >> slope >> intercept;

            std::remove ("tmp_data");
            std::remove ("py_out");

            return {intercept, slope};

//            double X_mean = std::accumulate(x_set.begin(), x_set.end(), 0.0) / x_set.size();;
//            double Y_mean = std::accumulate(y_set.begin(), y_set.end(), 0.0) / y_set.size();
//            double SS_xy = 0;
//            double SS_xx = 0;
//            int n = x_set.size();
//            {
//                std::vector<T> temp;
//                for (int i = 0; i < x_set.size(); ++i) {
//                    temp.push_back (x_set.at (i) * y_set.at (i));
//                }
//                SS_xy = std::accumulate(temp.begin(), temp.end(), 0);
//                SS_xy = SS_xy - n * X_mean * Y_mean;
//            }
//            {
//                std::vector<T> temp;
//                for (int i = 0; i < x_set.size(); ++i) {
//                    temp.push_back (x_set.at (i) * x_set.at (i));
//                }
//                SS_xx = std::accumulate(temp.begin(), temp.end(), 0);
//                SS_xx = SS_xx - n * X_mean * X_mean;
//            }
//            auto B_1 =  SS_xy / SS_xx;
//            auto B_0 =  Y_mean - B_1 * X_mean ;
//            std::pair <double, double> res = std::make_pair (B_0, B_1);
//
//            return res;


        }
    };
}


#endif //EVALUATION_UTILS_HPP
