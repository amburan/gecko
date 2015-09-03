[Mesh]
  type = FileMesh
  file = te_mesh_v1.e
[]

[MeshModifiers]
  active = ''
  [./ptype]
    type = SubdomainBoundingBox
    bottom_left = '0.0 0.1 0.0'
    top_right = '1.2 1.1 0.0'
    block_name = ptype
    block_id = 1
  [../]
  [./hotside]
    type = SubdomainBoundingBox
    bottom_left = '0 1.1 0'
    top_right = '2.7 1.2 0'
    block_name = hotside
    block_id = 2
  [../]
  [./left_cold_side]
    type = SubdomainBoundingBox
    bottom_left = '0.0 0.0 0.0'
    top_right = '1.2 0.1 0.0'
    block_name = left_cold_side
    block_id = 0
  [../]
  [./right_cold_side]
    type = SubdomainBoundingBox
    bottom_left = '1.7 0.0 0.0'
    top_right = '2.7 0.1 0.0'
    block_name = right_cold_side
    block_id = 4
  [../]
  [./ntype]
    type = SubdomainBoundingBox
    bottom_left = '1.7 0.1 0.0'
    top_right = '2.7 1.1 0.0'
    block_name = ntype
    block_id = 3
  [../]
  [./h_side]
    type = SideSetsAroundSubdomain
    new_boundary = h_side
    block = 2
    normal = '0 1 0'
  [../]
  [./c_side_left]
    type = SideSetsAroundSubdomain
    new_boundary = c_side_left
    block = 0
    normal = '0 -1 0'
  [../]
  [./c_side_right]
    type = SideSetsAroundSubdomain
    new_boundary = c_side_right
    block = 4
    normal = '0 -1 0'
  [../]
[]

[Variables]
  [./temp]
    initial_condition = 300
  [../]
[]

[Kernels]
  [./hc]
    type = HeatConduction
    variable = temp
    block = '0 1 2 3 4'
  [../]
  [./hc_dt]
    type = HeatConductionTimeDerivative
    variable = temp
  [../]
[]

[BCs]
  [./hot_side_bc]
    type = DirichletBC
    variable = temp
    boundary = h_side
    value = 350
  [../]
  [./cold_side]
    type = DirichletBC
    variable = temp
    boundary = 'c_side_left c_side_right'
    value = 300
  [../]
[]

[Materials]
  [./Copper]
    type = GenericConstantMaterial
    block = 'hotside left_cold_side right_cold_side'
    prop_names = 'thermal_conductivity specific_heat density' # [W/(m.K)], [J/(kg.K)]@ 25 C, [kg/m3]
    prop_values = '350 385 8940'
  [../]
  [./p_type_mat]
    type = GenericConstantMaterial
    block = ptype
    prop_names = 'thermal_conductivity specific_heat density' # [W/(m.K)][] [kg/m3]]
    prop_values = '1.2 155.875 7858.7' # denisty:http://scholarworks.uno.edu/cgi/viewcontent.cgi?article=2513&context=td
  [../]
  [./n_type_mat]
    type = GenericConstantMaterial
    block = ntype
    prop_names = 'thermal_conductivity specific_heat density' # [W/(m.K)]
    prop_values = '1.4 135.875 7858.7'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 200
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  csv = true
  [./out]
    type = Exodus
    output_initial = true
  [../]
[]

[Problem]
  type = FEProblem
[]
