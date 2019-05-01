truth_d1 = load('/localhome/ryan_watson/test/drives/truth/drive_1.xyz');
truth_d2 = load('/localhome/ryan_watson/test/drives/truth/drive_2.xyz');
truth_d3 = load('/localhome/ryan_watson/test/drives/truth/drive_3.xyz');

nom = [856514.1467,-4843013.0689, 4047939.8237];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%% GET BCE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
bce_d1_lq = load('/localhome/ryan_watson/test/drives/lq/bce/01/ecef.sol');
[~, bce_d1lq_rh, bce_d1lq_rt] = getError(truth_d1, bce_d1_lq, truth_d1(1,2:end));

bce_d2_lq = load('/localhome/ryan_watson/test/drives/lq/bce/02/ecef.sol');
[~, bce_d2lq_rh, bce_d2lq_rt] = getError(truth_d2, bce_d2_lq, truth_d2(1,2:end));

bce_d3_lq = load('/localhome/ryan_watson/test/drives/lq/bce/03/ecef.sol');
[~, bce_d3lq_rh, bce_d3lq_rt] = getError(truth_d3, bce_d3_lq, truth_d3(1,2:end));

bce_d1_hq = load('/localhome/ryan_watson/test/drives/hq/bce/01/ecef.sol');
[~, bce_d1hq_rh, bce_d1hq_rt] = getError(truth_d1, bce_d1_hq, truth_d1(1,2:end));

bce_d2_hq = load('/localhome/ryan_watson/test/drives/hq/bce/02/ecef.sol');
[~, bce_d2hq_rh, bce_d2hq_rt] = getError(truth_d2, bce_d2_hq, truth_d2(1,2:end));

bce_d3_hq = load('/localhome/ryan_watson/test/drives/hq/bce/03/ecef.sol');
[~, bce_d3hq_rh, bce_d3hq_rt] = getError(truth_d3, bce_d3_hq, truth_d3(1,2:end));

% BCE Hor
dataset = {'D1_LQ';'D1_HQ';'D2_LQ';'D2_HQ';'D3_LQ';'D3_HQ'};
med_ = [median(bce_d1lq_rh);median(bce_d1hq_rh); median(bce_d2lq_rh); median(bce_d2hq_rh); median(bce_d3lq_rh); median(bce_d3hq_rh)];
mean_ = [mean(bce_d1lq_rh);mean(bce_d1hq_rh); mean(bce_d2lq_rh); mean(bce_d2hq_rh); mean(bce_d3lq_rh); mean(bce_d3hq_rh)];
var_ = [var(bce_d1lq_rh);var(bce_d1hq_rh); var(bce_d2lq_rh); var(bce_d2hq_rh); var(bce_d3lq_rh); var(bce_d3hq_rh)];
max_ = [max(bce_d1lq_rh);max(bce_d1hq_rh); max(bce_d2lq_rh); max(bce_d2hq_rh); max(bce_d3lq_rh); max(bce_d3hq_rh)];

Batch_Covariance_Results = table(dataset,med_, mean_, var_, max_)

tmp_bce_1 = med_;
tmp_bce_2 = mean_;
tmp_bce_3 = var_;
tmp_bce_4 = max_;




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%% GET L2 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
l2_d1_lq = load('/localhome/ryan_watson/test/drives/lq/l2/01/ecef.sol');
[~, l2_d1lq_rh, l2_d1lq_rt] = getError(truth_d1, l2_d1_lq, truth_d1(1,2:end));

l2_d2_lq = load('/localhome/ryan_watson/test/drives/lq/l2/02/ecef.sol');
[~, l2_d2lq_rh, l2_d2lq_rt] = getError(truth_d2, l2_d2_lq, truth_d2(1,2:end));

l2_d3_lq = load('/localhome/ryan_watson/test/drives/lq/l2/03/ecef.sol');
[~, l2_d3lq_rh, l2_d3lq_rt] = getError(truth_d3, l2_d3_lq, truth_d3(1,2:end));

l2_d1_hq = load('/localhome/ryan_watson/test/drives/hq/l2/01/ecef.sol');
[~, l2_d1hq_rh, l2_d1hq_rt] = getError(truth_d1, l2_d1_hq, truth_d1(1,2:end));

l2_d2_hq = load('/localhome/ryan_watson/test/drives/hq/l2/02/ecef.sol');
[~, l2_d2hq_rh, l2_d2hq_rt] = getError(truth_d2, l2_d2_hq, truth_d2(1,2:end));

l2_d3_hq = load('/localhome/ryan_watson/test/drives/hq/l2/03/ecef.sol');
[~, l2_d3hq_rh, l2_d3hq_rt] = getError(truth_d3, l2_d3_hq, truth_d3(1,2:end));





% L2 Hor
dataset = {'D1_LQ';'D1_HQ';'D2_LQ';'D2_HQ';'D3_LQ';'D3_HQ'};
med_ = [median(l2_d1lq_rh);median(l2_d1hq_rh); median(l2_d2lq_rh); median(l2_d2hq_rh); median(l2_d3lq_rh); median(l2_d3hq_rh)];
mean_ = [mean(l2_d1lq_rh);mean(l2_d1hq_rh); mean(l2_d2lq_rh); mean(l2_d2hq_rh); mean(l2_d3lq_rh); mean(l2_d3hq_rh)];
var_ = [var(l2_d1lq_rh);var(l2_d1hq_rh); var(l2_d2lq_rh); var(l2_d2hq_rh); var(l2_d3lq_rh); var(l2_d3hq_rh)];
max_ = [max(l2_d1lq_rh);max(l2_d1hq_rh); max(l2_d2lq_rh); max(l2_d2hq_rh); max(l2_d3lq_rh); max(l2_d3hq_rh)];

L2_Results = table(dataset,med_, mean_, var_, max_)

tmp_l2_1 = med_;
tmp_l2_2 = mean_;
tmp_l2_3 = var_;
tmp_l2_4 = max_;



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%% GET DCS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
dcs_d1_lq = load('/localhome/ryan_watson/test/drives/lq/dcs/01/ecef.sol');
[~, dcs_d1lq_rh, dcs_d1lq_rt] = getError(truth_d1, dcs_d1_lq, truth_d1(1,2:end));

dcs_d2_lq = load('/localhome/ryan_watson/test/drives/lq/dcs/02/ecef.sol');
[~, dcs_d2lq_rh, dcs_d2lq_rt] = getError(truth_d2, dcs_d2_lq, truth_d2(1,2:end));

dcs_d3_lq = load('/localhome/ryan_watson/test/drives/lq/dcs/03/ecef.sol');
[~, dcs_d3lq_rh, dcs_d3lq_rt] = getError(truth_d3, dcs_d3_lq, truth_d3(1,2:end));

dcs_d1_hq = load('/localhome/ryan_watson/test/drives/hq/dcs/01/ecef.sol');
[~, dcs_d1hq_rh, dcs_d1hq_rt] = getError(truth_d1, dcs_d1_hq, truth_d1(1,2:end));

dcs_d2_hq = load('/localhome/ryan_watson/test/drives/hq/dcs/02/ecef.sol');
[~, dcs_d2hq_rh, dcs_d2hq_rt] = getError(truth_d2, dcs_d2_hq, truth_d2(1,2:end));

dcs_d3_hq = load('/localhome/ryan_watson/test/drives/hq/dcs/03/ecef.sol');
[~, dcs_d3hq_rh, dcs_d3hq_rt] = getError(truth_d3, dcs_d3_hq, truth_d3(1,2:end));


% L2 Hor
dataset = {'D1_LQ';'D1_HQ';'D2_LQ';'D2_HQ';'D3_LQ';'D3_HQ'};
med_ = [median(dcs_d1lq_rh);median(dcs_d1hq_rh); median(dcs_d2lq_rh); median(dcs_d2hq_rh); median(dcs_d3lq_rh); median(dcs_d3hq_rh)];
mean_ = [mean(dcs_d1lq_rh);mean(dcs_d1hq_rh); mean(dcs_d2lq_rh); mean(dcs_d2hq_rh); mean(dcs_d3lq_rh); mean(dcs_d3hq_rh)];
var_ = [var(dcs_d1lq_rh);var(dcs_d1hq_rh); var(dcs_d2lq_rh); var(dcs_d2hq_rh); var(dcs_d3lq_rh); var(dcs_d3hq_rh)];
max_ = [max(dcs_d1lq_rh);max(dcs_d1hq_rh); max(dcs_d2lq_rh); max(dcs_d2hq_rh); max(dcs_d3lq_rh); max(dcs_d3hq_rh)];

DCS_Results = table(dataset,med_, mean_, var_, max_)

tmp_dcs_1 = med_;
tmp_dcs_2 = mean_;
tmp_dcs_3 = var_;
tmp_dcs_4 = max_;





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%% GET MM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mm_d1_lq = load('/localhome/ryan_watson/test/drives/lq/mm/01/ecef.sol');
[~, mm_d1lq_rh, mm_d1lq_rt] = getError(truth_d1, mm_d1_lq, truth_d1(1,2:end));

mm_d2_lq = load('/localhome/ryan_watson/test/drives/lq/mm/02/ecef.sol');
[~, mm_d2lq_rh, mm_d2lq_rt] = getError(truth_d2, mm_d2_lq, truth_d2(1,2:end));

mm_d3_lq = load('/localhome/ryan_watson/test/drives/lq/mm/03/ecef.sol');
[~, mm_d3lq_rh, mm_d3lq_rt] = getError(truth_d3, mm_d3_lq, truth_d3(1,2:end));

mm_d1_hq = load('/localhome/ryan_watson/test/drives/hq/mm/01/ecef.sol');
[~, mm_d1hq_rh, mm_d1hq_rt] = getError(truth_d1, mm_d1_hq, truth_d1(1,2:end));

mm_d2_hq = load('/localhome/ryan_watson/test/drives/hq/mm/02/ecef.sol');
[~, mm_d2hq_rh, mm_d2hq_rt] = getError(truth_d2, mm_d2_hq, truth_d2(1,2:end));

mm_d3_hq = load('/localhome/ryan_watson/test/drives/hq/mm/03/ecef.sol');
[~, mm_d3hq_rh, mm_d3hq_rt] = getError(truth_d3, mm_d3_hq, truth_d3(1,2:end));



% mm Hor
dataset = {'D1_LQ';'D1_HQ';'D2_LQ';'D2_HQ';'D3_LQ';'D3_HQ'};
med_ = [median(mm_d1lq_rh);median(mm_d1hq_rh); median(mm_d2lq_rh); median(mm_d2hq_rh); median(mm_d3lq_rh); median(mm_d3hq_rh)];
mean_ = [mean(mm_d1lq_rh);mean(mm_d1hq_rh); mean(mm_d2lq_rh); mean(mm_d2hq_rh); mean(mm_d3lq_rh); mean(mm_d3hq_rh)];
var_ = [var(mm_d1lq_rh);var(mm_d1hq_rh); var(mm_d2lq_rh); var(mm_d2hq_rh); var(mm_d3lq_rh); var(mm_d3hq_rh)];
max_ = [max(mm_d1lq_rh);max(mm_d1hq_rh); max(mm_d2lq_rh); max(mm_d2hq_rh); max(mm_d3lq_rh); max(mm_d3hq_rh)];

mm_Results = table(dataset,med_, mean_, var_, max_)

tmp_mm_1 = med_;
tmp_mm_2 = mean_;
tmp_mm_3 = var_;
tmp_mm_4 = max_;
