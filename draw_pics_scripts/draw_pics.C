// plot_from_root.C
// 本宏演示如何读取保存为标量（非 vector）的 ROOT 文件中的数据，并绘制图形

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMath.h>
#include <iostream>

void draw_plot() {
    // 打开 ROOT 文件
    TFile *f = TFile::Open("/home/xyqin/scratch/B8_solar_nue/dl_root_files_v1/test_set_DL_v1.root");
    if (!f || f->IsZombie()) {
        std::cerr << "无法打开 ROOT 文件 test_set_DL_v1.root" << std::endl;
        return;
    }
    
    // 获取 TTree 对象，树名需与保存时一致（例如 "event_ntuple"）
    TTree *tree = (TTree*)f->Get("event_ntuple");
    if (!tree) {
        std::cerr << "没有找到树 event_ntuple" << std::endl;
        return;
    }
    
    // 假设各分支为标量类型（float）
    Float_t dl_x = 0, dl_y = 0, dl_z = 0;
    Float_t sf_x = 0, sf_y = 0, sf_z = 0;
    Float_t real_x = 0, real_y = 0, real_z = 0;
    Float_t energy = 0, nhits = 0;
    
    // 设置分支地址
    tree->SetBranchAddress("dl_position_x", &dl_x);
    tree->SetBranchAddress("dl_position_y", &dl_y);
    tree->SetBranchAddress("dl_position_z", &dl_z);
    tree->SetBranchAddress("real_position_x", &real_x);
    tree->SetBranchAddress("real_position_y", &real_y);
    tree->SetBranchAddress("real_position_z", &real_z);
    tree->SetBranchAddress("sf_position_x", &sf_x);
    tree->SetBranchAddress("sf_position_y", &sf_y);
    tree->SetBranchAddress("sf_position_z", &sf_z);
    tree->SetBranchAddress("nhits", &nhits);
    tree->SetBranchAddress("energy", &energy);

    int nEntries = tree->GetEntries();
    std::cout << "总共事件数： " << nEntries << std::endl;
    
    // 1. 绘制 3D 距离直方图（DL 与真实位置之间的欧式距离）
    TH1D *dl_hDist = new TH1D("hDist", "DL vs Real 3D Distance;Distance (mm);Entries", 100, 0, 1000);
    TH1D *sf_hDist = new TH1D("hDist", "SF vs Real 3D Distance;Distance (mm);Entries", 100, 0, 1000);
    // Define TGraph objects for line plots (or scatter plots)
    // Note: TGraph typically plots y vs x. The names suggest plotting
    // variance (distance) vs energy or nhits.
    // Initialize empty TGraphs; points will be added in the loop.
    TGraph *g_energy_vs_sf = new TGraph();
    TGraph *g_energy_vs_dl = new TGraph();
    TGraph *g_nhits_vs_sf = new TGraph();
    TGraph *g_nhits_vs_dl = new TGraph();
    
    int idx_dl = 0, idx_sf = 0, idx_nhits_dl = 0, idx_nhits_sf = 0;
    for (int i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        double dl_dist = TMath::Sqrt((dl_x - real_x) * (dl_x - real_x) +
                                     (dl_y - real_y) * (dl_y - real_y) +
                                     (dl_z - real_z) * (dl_z - real_z));
        double sf_dist = TMath::Sqrt((sf_x - real_x) * (sf_x - real_x) +
                                     (sf_y - real_y) * (sf_y - real_y) +
                                     (sf_z - real_z) * (sf_z - real_z));
        dl_hDist->Fill(dl_dist);
        sf_hDist->Fill(sf_dist);

        // 填充散点图
        g_energy_vs_dl->SetPoint(idx_dl++, energy, dl_dist);
        g_energy_vs_sf->SetPoint(idx_sf++, energy, sf_dist);
        g_nhits_vs_dl->SetPoint(idx_nhits_dl++, nhits, dl_dist);
        g_nhits_vs_sf->SetPoint(idx_nhits_sf++, nhits, sf_dist);
    }
    
    TCanvas *c1 = new TCanvas("c1", "3D Distance Distribution", 1000, 800);

// 设置 DL 结果样式
dl_hDist->SetLineColor(kBlue);
dl_hDist->SetLineWidth(2);
dl_hDist->SetStats(0);
dl_hDist->SetTitle("3D Distance Distribution;Distance (mm);Entries");
// 设置 Y 轴最大值为 histogram 最大 bin 值的 1.2 倍
double maxY = std::max(dl_hDist->GetMaximum(), sf_hDist->GetMaximum());
dl_hDist->SetMaximum(1.1 * maxY);  // 设置 Y 轴上限

dl_hDist->Draw();  // 先画 DL

// 设置 SF 结果样式
sf_hDist->SetLineColor(kRed);
sf_hDist->SetLineWidth(2);
sf_hDist->SetStats(0);
sf_hDist->Draw("SAME");  // 叠加画上去

// 添加图例
TLegend *legend = new TLegend(0.6, 0.75, 0.88, 0.88);  // 调整位置
legend->AddEntry(dl_hDist, "Deep learning result", "l");
legend->AddEntry(sf_hDist, "scintFitter result", "l");
legend->SetBorderSize(0);
legend->SetFillStyle(0);
legend->Draw();

// 计算均值和标准差
double dl_mean = dl_hDist->GetMean();
double dl_std  = dl_hDist->GetStdDev();
double sf_mean = sf_hDist->GetMean();
double sf_std  = sf_hDist->GetStdDev();

// 添加文本显示均值和标准差
TPaveText *pt = new TPaveText(0.6, 0.58, 0.88, 0.73, "NDC");  // NDC 表示归一化坐标
pt->SetFillColor(0);  // 背景透明
pt->SetTextAlign(12); // 左对齐
pt->SetTextFont(42);
pt->AddText(Form("DL mean = %.2f mm", dl_mean));
pt->AddText(Form("DL std  = %.2f mm", dl_std));
pt->AddText(Form("SF mean = %.2f mm", sf_mean));
pt->AddText(Form("SF std  = %.2f mm", sf_std));
pt->Draw();

c1->SaveAs("distance_distribution_comp.png");

    
    // 2. 绘制散点图，对比真实 X 坐标与 DL 预测 X 坐标
    TGraph *gr = new TGraph(nEntries);
    int pointIndex = 0;
    for (int i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        // 在这里假设每个事件的 DL 与 Real X 坐标是有效数据
        gr->SetPoint(pointIndex, real_x, dl_x);
        pointIndex++;
    }
    
    TCanvas *c2 = new TCanvas("c2", "Predicted vs Real X Position", 800, 600);
    gr->SetTitle("Predicted vs Real X;Real X (mm);Predicted X (mm)");
    gr->Draw("AP");
    c2->SaveAs("pred_vs_real_x.png");

    // 3. 绘制能量 vs DL重建误差的散点图
    TCanvas *c3 = new TCanvas("c3", "Energy vs DL Distance", 800, 600);
    g_energy_vs_dl->SetTitle("Energy vs DL 3D Distance;Energy (MeV);DL 3D Distance (mm)");
    g_energy_vs_dl->SetMarkerStyle(20);
    g_energy_vs_dl->SetMarkerSize(0.3);
    g_energy_vs_dl->SetMarkerColor(kBlue+2);
    g_energy_vs_dl->Draw("AP");
    c3->SaveAs("energy_vs_dl_dist.png");

    // 4. 绘制能量 vs SF重建误差的散点图
    TCanvas *c4 = new TCanvas("c4", "Energy vs SF Distance", 800, 600);
    g_energy_vs_sf->SetTitle("Energy vs SF 3D Distance;Energy (MeV);SF 3D Distance (mm)");
    g_energy_vs_sf->SetMarkerStyle(20);
    g_energy_vs_sf->SetMarkerSize(0.3);
    g_energy_vs_sf->SetMarkerColor(kRed+2);
    g_energy_vs_sf->Draw("AP");
    c4->SaveAs("energy_vs_sf_dist.png");

    // 5. 绘制 nhits vs DL重建误差的散点图
    TCanvas *c5 = new TCanvas("c5", "nhits vs DL Distance", 800, 600);
    g_nhits_vs_dl->SetTitle("nhits vs DL 3D Distance;nhits;DL 3D Distance (mm)");
    g_nhits_vs_dl->SetMarkerStyle(20);
    g_nhits_vs_dl->SetMarkerSize(0.3);
    g_nhits_vs_dl->SetMarkerColor(kBlue+2);
    g_nhits_vs_dl->Draw("AP");
    c5->SaveAs("nhits_vs_dl_dist.png");

    // 6. 绘制 nhits vs SF重建误差的散点图
    TCanvas *c6 = new TCanvas("c6", "nhits vs SF Distance", 800, 600);
    g_nhits_vs_sf->SetTitle("nhits vs SF 3D Distance;nhits;SF 3D Distance (mm)");
    g_nhits_vs_sf->SetMarkerStyle(20);
    g_nhits_vs_sf->SetMarkerSize(0.3);
    g_nhits_vs_sf->SetMarkerColor(kRed+2);
    g_nhits_vs_sf->Draw("AP");
    c6->SaveAs("nhits_vs_sf_dist.png");
    
    // 关闭 ROOT 文件
    f->Close();
    
    std::cout << "绘图完成，生成 distance_distribution.png， pred_vs_real_x.png 和 energy_vs_dl_dist.png" << std::endl;
}
