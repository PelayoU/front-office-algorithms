# Front-Office Algorithms


Front-office quantitative work — a C++ valuation engine for fixed-income, and an xVA case study on a four-deal Monte Carlo book under five different CSA mitigant regimes. The headline project prices bonds and interest-rate swaps off a continuously-compounded zero-coupon curve; the companion case study quantifies CVA, DVA and FVA across the mitigant ladder (no agreement → netting → asymmetric CSA → symmetric CSA → strict CSA) using the sign-weighted exposure formula required when NPV crosses zero within a tramo.

---

## Project index

| Project | Stack | Focus |
| :--- | :--- | :--- |
| **[Bonds, Swaps and Curve Calibration Engine](./bonds-swaps-and-curve-calibration-engine)** | C++14 · Boost.Test · Boost.Date_Time · CMake | Fixed-income valuation engine: zero-coupon discount curves, IRS pricing on Euribor 6M, par-swap rate, off-market MtM, bond cash-flow discounting, iterative bootstrapping (deposit + par swaps). Factory pattern with builder registration. **62 Boost.Test cases passing**, every numeric tied to a worked example. |
| **[xVA — Credit and Funding Valuation Adjustments](./xva-credit-and-funding-adjustments)** | Excel · Monte Carlo | CVA / DVA / FVA on a four-deal book under five mitigant sets (no agreement → netting → asymmetric CSA → symmetric CSA → strict CSA). Sign-weighted exposure formula for opposite-sign tramos, full collateral-account roll under Threshold / MTA rules, MPR-style effective-NPV during each interval, FVA bonus track at 1 % remuneration / 1.5 % funding cost, written analysis of the comparative results. |

---

## Areas of focus

- **Fixed-income pricing** — discounted cash flows on a continuously-compounded zero curve, par-swap rate computation, off-market swap MtM.
- **Curve calibration** — iterative bootstrapping of discount factors from deposits and par swaps, node-by-node closed-form solution, day-count conventions (`Actual/360`, `30/360`).
- **Counterparty risk and xVA** — bilateral CVA / DVA / FVA on a Monte Carlo book, opposite-sign exposure weighting, CSA collateral mechanics (Threshold, MTA, asymmetric vs symmetric posting), margin-period-of-risk effective exposure, funding benefit / cost decomposition.
- **Modern C++ design** — Rule-of-Five RAII, Factory + registrator pattern, value-semantics descriptions, `boost::gregorian::date` for date arithmetic.
- **Testing discipline** — Boost.Test suites anchored to canonical worked tables, with micro-tolerances on the reference numerics (1e-15 on the bond example).

---

## Author

**Pelayo Urzaiz**

- BSc in Applied Statistics — Universidad Complutense de Madrid (UCM)
- MSc in Financial Technologies (FinTech) — Universidad Carlos III de Madrid (UC3M)
- MSc in Quantitative Finance — Universidad Nacional de Educación a Distancia (UNED)

[LinkedIn Profile](https://www.linkedin.com/in/pelayourzaiz/)
