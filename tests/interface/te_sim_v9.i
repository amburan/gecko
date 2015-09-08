[Mesh]
  type = FileMesh
  file = te_mesh_v8.e
[]

[MeshModifiers]
  active = 'tbc_sideset bbc_sideset'
  [./left_cold_block]
    type = SubdomainBoundingBox
    bottom_left = '0.0 0.0 0.0'
    top_right = '0.00125 0.0001 0.0'
    block_name = left_cold_block
    block_id = 0
  [../]
  [./ptype_block]
    type = SubdomainBoundingBox
    bottom_left = '0.0, 0.0001, 0.0'
    top_right = '0.00125, 0.0011, 0.0'
    block_name = ptype_block
    block_id = 1
  [../]
  [./hotside_block]
    type = SubdomainBoundingBox
    bottom_left = '0.0, 0.0011, 0.0'
    top_right = '0.00274, 0.0012, 0.0'
    block_name = hotside_block
    block_id = 2
  [../]
  [./ntype_block]
    type = SubdomainBoundingBox
    bottom_left = '0.00174, 0.0001, 0.0'
    top_right = '0.00274, 0.0011, 0.0'
    block_name = ntype_block
    block_id = 3
  [../]
  [./right_cold_block]
    type = SubdomainBoundingBox
    bottom_left = '0.00174, 0.0, 0.0'
    top_right = '0.00274, 0.0001, 0.0'
    block_name = right_cold_block
    block_id = 4
  [../]
  [./cold_left_sideset]
    type = SideSetsAroundSubdomain
    new_boundary = cold_left_sideset
    block = left_cold_block
    normal = '0 -1 0'
  [../]
  [./hot_sideset]
    type = SideSetsAroundSubdomain
    new_boundary = hot_sideset
    block = hotside_block
    normal = '0 1 0'
  [../]
  [./cold_right_sideset]
    type = SideSetsAroundSubdomain
    new_boundary = cold_right_sideset
    block = right_cold_block
    normal = '0 -1 0'
  [../]
  [./atc_block]
    type = SubdomainBoundingBox
    bottom_left = '0.000625, 0.001025, 0.0'
    top_right = '0.000700, 0.001175, 0.0'
    block_name = atc_block
    block_id = 5
  [../]
  [./md_nodeset]
    type = BoundingBoxNodeSet
    top_right = '0.0007, 0.001125, 0.0'
    new_boundary = md_nodeset
    bottom_left = '0.000625, 0.001075, 0.0'
  [../]
  [./tbc_sideset]
    type = SideSetsAroundSubdomain
    new_boundary = tbc_sideset
    block = atc_block
    normal = '0 1 0'
  [../]
  [./bbc_sideset]
    type = SideSetsAroundSubdomain
    new_boundary = bbc_sideset
    block = atc_block
    normal = '0 -1 0'
  [../]
[]

[Variables]
  active = 'temp'
  [./temp]
    initial_condition = 300
  [../]
  [./volt]
    initial_condition = 0
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
  [./hotside_BC]
    type = DirichletBC
    variable = temp
    boundary = 8
    value = 350
  [../]
  [./coldside_BC]
    type = DirichletBC
    variable = temp
    boundary = '7 9'
    value = 300
  [../]
[]

[Materials]
  [./Copper]
    type = GenericConstantMaterial
    block = 'hotside_block left_cold_block right_cold_block'
    prop_names = 'thermal_conductivity specific_heat density' # [W/(m.K)], [J/(kg.K)]@ 25 C, [kg/m3]
    prop_values = '350 385 8940'
  [../]
  [./p_type_mat]
    type = GenericConstantMaterial
    block = ptype_block
    prop_names = 'thermal_conductivity specific_heat density' # [W/(m.K)][] [kg/m3]]
    prop_values = '1.2 155.875 7858.7' # denisty:http://scholarworks.uno.edu/cgi/viewcontent.cgi?article=2513&context=td
  [../]
  [./n_type_mat]
    type = GenericConstantMaterial
    block = ntype_block
    prop_names = 'thermal_conductivity specific_heat density' # [W/(m.K)]
    prop_values = '1.4 135.875 7858.7'
  [../]
  [./dummy_mat]
    type = GenericConstantMaterial
    block = 5
  [../]
[]

[Postprocessors]
  [./tbc]
    # top boundary condition. Used for setting BC for lammps AtC
    type = SideAverageValue
    variable = temp
    execute_on = 'timestep_begin timestep_end'
    boundary = tbc_sideset
  [../]
  [./bbc]
    # bottom boundary condition. Used for setting BC for lammps AtC
    type = SideAverageValue
    variable = temp
    execute_on = 'timestep_begin timestep_end'
    boundary = bbc_sideset
  [../]
  [./avg_atomic_temp]
    # For calculating average temperature in area where coupling is enabled.
    type = AverageNodalVariableValue
    variable = temp
    boundary = 10
  [../]
[]

[UserObjects]
  [./lammps_uo]
    type = LammpsUserObject
    rightDownScalingTemperature = bbc
    leftDownScalingTemperature = tbc
    execute_on = 'TIMESTEP_END timestep_begin'
    LammpsTimeSteps = 1000
    lammpsEquilibriationInput = in.te_v2
  [../]
[]

[Problem]
  type = FEProblem
[]

[Executioner]
  type = Transient
  trans_ss_check = true
  num_steps = 2000
  dt = 0.01
  ss_check_tol = 1.0e-04
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
  [./te_sim]
    type = Exodus
  [../]
[]
