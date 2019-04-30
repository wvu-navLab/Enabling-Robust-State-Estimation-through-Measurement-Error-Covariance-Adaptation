figure;
hold on;
set(gcf, 'renderer', 'Painters')

res = load('hq/bce/01/iter_0.residuals');


S = scatter(res(:,1),res(:,2),30,'MarkerEdgeColor','k',...
              'MarkerFaceColor','k',...
              'LineWidth',1.5)

alpha = 0.5;

weight_out = 0.164493;
weight_in = 0.835487;
mean_out = [-0.42729 0.000537972];
mean_in =  [0.0210119 -6.65043e-05]
cov_out = [22.8517 -0.00716079; -0.00716079 0.000768311];
cov_in = [1.61554 -0.00120016; -0.00120016 0.000168894];

L(1) = error_ellipse(cov_out,mean_out,0.95);
L(2) = error_ellipse(cov_in, mean_in, 0.95);

color = ['r','g'];
P(1) = patch(get(L(1),'XData'),get(L(1),'YData'),color(1),'FaceAlpha',.35);
P(2) = patch(get(L(2),'XData'),get(L(2),'YData'),color(2),'FaceAlpha',.35);

delete(L(1));
uistack(P(1),'top');

delete(L(2));
uistack(P(2),'top');

ylabel('\Phi Residuals ( m. )')
xlabel('\rho Residuals ( m. )')
set(gca, 'FontName', 'Times New Roman')
set(gca, 'FontWeight', 'bold')
set(findall(gcf,'-property','FontSize'),'FontSize',24)
