#include "the4.h"
#define MAX_DIM 601
#define INF 0x3f3f3f3f

int divide_land(int X, int Y, bool** possible_plots) 
{
    int dp[MAX_DIM][MAX_DIM];

    for (int i = 0; i <= X; ++i) 
    {
        for (int j = 0; j <= Y; ++j) 
        {
            dp[i][j] = INF;
        }
    }

    dp[0][0] = 0;

    for (int i = 1; i <= X; ++i) 
    {
        for (int j = 1; j <= Y; ++j) 
        {
            if (possible_plots[i][j]) 
            {
                dp[i][j] = 0;
            } else 
            {
                dp[i][j] = i * j;
                for (int k = 1; k <= i / 2; ++k) 
                {
                    dp[i][j] = (dp[i][j] < dp[k][j] + dp[i - k][j]) ? dp[i][j] : (dp[k][j] + dp[i - k][j]);
                }
                for (int k = 1; k <= j / 2; ++k) 
                {
                    dp[i][j] = (dp[i][j] < dp[i][k] + dp[i][j - k]) ? dp[i][j] : (dp[i][k] + dp[i][j - k]);
                }
            }
        }
    }

    return dp[X][Y];
}
